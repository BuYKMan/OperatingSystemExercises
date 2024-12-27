/**
 * Grup: 30
 * Bu dosya komutların çalıştırdığımız ve sinyal takibi yaptığımız dosyadır
 *
 * Yazanlar:
 * Samed Değer - B211210022
 * Saad Saad -B2112110552
 * FUAD ABDULLAH YAHYA AISHAN G201210562
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include "executor.h"
#include "utils.h"

extern int background_processes_count;
extern int increment_command(void);

int execute_built_in_command(Command *cmd)
{
    if (cmd->args[0] == NULL)
        return -1;

    if (strcmp(cmd->args[0], "increment") == 0)
    {
        return increment_command();
    }
    if (strcmp(cmd->args[0], "quit") == 0)
    {
        return -2; // Özel quit sinyali
    }
    return -1; // Built-in komut değil
}

int execute_command(Command *cmd)
{
    if (cmd->pipe_count > 0)
    {
        return execute_pipe_commands(cmd);
    }
    else
    {
        return execute_single_command(cmd);
    }
}

int execute_single_command(Command *cmd)
{
    // Built-in komut kontrolü
    int built_in_result = execute_built_in_command(cmd);
    if (built_in_result >= 0)
    { // Built-in komut çalıştı
        return built_in_result;
    }
    if (built_in_result == -2)
    { // Quit komutu
        exit(0);
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return -1;
    }

    if (pid == 0)
    { // Çocuk proses
        // I/O yönlendirmelerini ayarla
        if (setup_redirections(cmd) != 0)
        {
            exit(1);
        }

        // Komutu çalıştır
        execvp(cmd->args[0], cmd->args);
        fprintf(stderr, "Komut çalıştırılamadı: %s\n", cmd->args[0]);
        exit(1);
    }

    // Ebeveyn proses
    if (!cmd->background)
    {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
    else
    {
        background_processes_count++;
        printf("[%d] Arkaplan işlemi başlatıldı\n", pid);
    }
    return 0;
}

int execute_pipe_commands(Command *cmd)
{
    int pipes[MAX_PIPES][2];
    pid_t pids[MAX_PIPES + 1];
    int i;

    // İlk komutun built-in olup olmadığını kontrol et
    int is_first_builtin = (strcmp(cmd->args[0], "increment") == 0);

    // Son komutun built-in olup olmadığını kontrol et
    int is_last_builtin = (strcmp(cmd->pipe_commands[cmd->pipe_count - 1][0], "increment") == 0);

    // Pipe'ları oluştur
    for (i = 0; i < cmd->pipe_count; i++)
    {
        if (pipe(pipes[i]) < 0)
        {
            perror("pipe");
            return -1;
        }
    }

    // İlk komut için process oluştur
    pids[0] = fork();
    if (pids[0] < 0)
    {
        perror("fork");
        return -1;
    }

    if (pids[0] == 0)
    { // Çocuk process
        // Giriş yönlendirmesi
        if (cmd->input_file)
        {
            int fd = open(cmd->input_file, O_RDONLY);
            if (fd < 0)
            {
                perror("open input");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        // Çıkışı pipe'a yönlendir
        dup2(pipes[0][1], STDOUT_FILENO);

        // Kullanılmayan pipe'ları kapat
        for (i = 0; i < cmd->pipe_count; i++)
        {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        if (is_first_builtin)
        {
            increment_command();
            exit(0);
        }
        else
        {
            execvp(cmd->args[0], cmd->args);
            perror("execvp");
            exit(1);
        }
    }

    // Ara komutlar için process'ler
    for (i = 1; i < cmd->pipe_count; i++)
    {
        pids[i] = fork();
        if (pids[i] < 0)
        {
            perror("fork");
            return -1;
        }

        if (pids[i] == 0)
        { // Çocuk process
            // Girişi önceki pipe'dan al
            dup2(pipes[i - 1][0], STDIN_FILENO);
            // Çıkışı sonraki pipe'a yönlendir
            dup2(pipes[i][1], STDOUT_FILENO);

            // Kullanılmayan pipe'ları kapat
            for (int j = 0; j < cmd->pipe_count; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(cmd->pipe_commands[i - 1][0], cmd->pipe_commands[i - 1]);
            perror("execvp");
            exit(1);
        }
    }

    // Son komut için process oluştur
    pids[cmd->pipe_count] = fork();
    if (pids[cmd->pipe_count] < 0)
    {
        perror("fork");
        return -1;
    }

    if (pids[cmd->pipe_count] == 0)
    { // Çocuk process
        // Çıkış yönlendirmesi
        if (cmd->output_file)
        {
            int fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0)
            {
                perror("open output");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        // Girişi son pipe'dan al
        dup2(pipes[cmd->pipe_count - 1][0], STDIN_FILENO);

        // Kullanılmayan pipe'ları kapat
        for (i = 0; i < cmd->pipe_count; i++)
        {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        if (is_last_builtin)
        {
            increment_command();
            exit(0);
        }
        else
        {
            execvp(cmd->pipe_commands[cmd->pipe_count - 1][0],
                   cmd->pipe_commands[cmd->pipe_count - 1]);
            perror("execvp");
            exit(1);
        }
    }

    // Ana process'te tüm pipe'ları kapat
    for (i = 0; i < cmd->pipe_count; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Arkaplan işlemi değilse tüm çocukları bekle
    if (!cmd->background)
    {
        for (i = 0; i <= cmd->pipe_count; i++)
        {
            waitpid(pids[i], NULL, 0);
        }
    }
    else
    {
        background_processes_count++;
        printf("[%d] Arkaplan pipe işlemi başlatıldı\n", pids[0]);
    }

    return 0;
}

int setup_redirections(Command *cmd)
{
    // Giriş yönlendirmesi
    if (cmd->input_file)
    {
        int fd = open(cmd->input_file, O_RDONLY);
        if (fd < 0)
        {
            fprintf(stderr, "Giriş dosyası bulunamadı: %s\n", cmd->input_file);
            return -1;
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    // Çıkış yönlendirmesi
    if (cmd->output_file)
    {
        int fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0)
        {
            fprintf(stderr, "Çıkış dosyası oluşturulamadı: %s\n", cmd->output_file);
            return -1;
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    return 0;
}
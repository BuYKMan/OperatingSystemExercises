/**
 * Grup: 30
 * Bu dosya Linux Shell uygulamasının ana program dosyasıdır.
 *
 * Yazanlar:
 * Samed Değer - B211210022
 * Saad Saad -B2112110552
 * FUAD ABDULLAH YAHYA AISHAN G201210562
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "parser.h"
#include "executor.h"
#include "utils.h"

#define MAX_INPUT_SIZE 1024
#define PROMPT "> "

volatile sig_atomic_t child_exit_status;
int background_processes_count = 0;

// Signal handler fonksiyonu
void handle_sigchld(int sig)
{
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if (WIFEXITED(status))
        {
            printf("\n[%d] retval: %d\n", pid, WEXITSTATUS(status));
            background_processes_count--;
        }
    }
}

// increment işlevi
int increment_command(void)
{
    char buffer[256];
    int number;

    if (fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
        if (sscanf(buffer, "%d", &number) == 1)
        {
            printf("%d\n", number + 1);
            return 0;
        }
    }
    fprintf(stderr, "Geçersiz girdi\n");
    return 1;
}

int main()
{
    char input[MAX_INPUT_SIZE];
    Command cmd;

    // Signal handler kurulumu
    struct sigaction sa;
    sa.sa_handler = &handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, 0) == -1)
    {
        error_exit("sigaction");
        exit(1);
    }

    // Ana döngü
    while (1)
    {
        printf(PROMPT);
        fflush(stdout);

        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL)
        {
            break;
        }

        // Satır sonu karakterini kaldır
        input[strcspn(input, "\n")] = 0;

        // Bos girdi kontrolü
        if (strlen(input) == 0)
        {
            continue;
        }

        // Quit komutu kontrolü
        if (strcmp(input, "quit") == 0)
        {
            if (background_processes_count > 0)
            {
                printf("background porcess is waiting \n");
                while (background_processes_count > 0)
                {
                    pause();
                }
            }
            break;
        }

        // Komutu ayrıştır
        if (parse_command(input, &cmd) != 0)
        {
            printf("Komut ayrıştırma hatası!\n");
            continue;
        }

        // Komutu çalıştır
        execute_command(&cmd);

        // Komut yapısını temizle
        clear_command(&cmd);
    }

    printf("Shell kapatılıyor...\n");
    return 0;
}
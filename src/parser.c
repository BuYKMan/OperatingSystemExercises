/**
 * Grup: [GRUP_NO]
 * Bu dosya Linux Shell parser sınıfıdır özel karakterlere göre
 * komutu bölüp bu özel karakterlere atadığımızı işlemlere göre
 * komutu daha efektif çalışması için executor'a gönderdiğimiz sınıftır
 *
 * Yazanlar:
 * Samed Değer - B211210022
 * Saad Saad -B2112110552
 * FUAD ABDULLAH YAHYA AISHAN G201210562
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "utils.h"

// Özel karakterleri kontrol etmek için yardımcı fonksiyon
static int is_special_char(char c)
{
    return (c == '<' || c == '>' || c == '|' || c == '&' || c == ';');
}

int parse_command(char *input, Command *cmd)
{
    char *token;
    char *saveptr;
    int i = 0;
    int current_pipe_cmd = 0;

    // Command yapısını sıfırla
    memset(cmd, 0, sizeof(Command));

    // İlk token'ı al
    token = strtok_r(input, " \t", &saveptr);

    while (token != NULL && i < MAX_ARGS - 1)
    {
        if (strcmp(token, "<") == 0)
        {
            // Giriş yönlendirme
            token = strtok_r(NULL, " \t", &saveptr);
            if (token == NULL)
            {
                fprintf(stderr, "Giriş yönlendirmesi için dosya adı eksik\n");
                return -1;
            }
            cmd->input_file = strdup(token);
        }
        else if (strcmp(token, ">") == 0)
        {
            // Çıkış yönlendirme
            token = strtok_r(NULL, " \t", &saveptr);
            if (token == NULL)
            {
                fprintf(stderr, "Çıkış yönlendirmesi için dosya adı eksik\n");
                return -1;
            }
            cmd->output_file = strdup(token);
        }
        else if (strcmp(token, "|") == 0)
        {
            // Pipe işlemi - mevcut komutun argümanlarını sonlandır
            if (cmd->pipe_count == 0)
            {
                cmd->args[i] = NULL;
                cmd->arg_count = i;
            }
            else
            {
                cmd->pipe_commands[current_pipe_cmd - 1][i] = NULL;
                cmd->pipe_arg_counts[current_pipe_cmd - 1] = i;
            }

            // Bir sonraki komutu almak için hazırlan
            cmd->pipe_count++;
            current_pipe_cmd++;
            i = 0;

            token = strtok_r(NULL, " \t", &saveptr);
            continue;
        }
        else if (strcmp(token, "&") == 0)
        {
            // Arkaplan işlemi
            cmd->background = 1;
            break;
        }
        else
        {
            // Normal argüman
            if (cmd->pipe_count == 0)
            {
                cmd->args[i] = strdup(token);
                i++;
            }
            else
            {
                cmd->pipe_commands[current_pipe_cmd - 1][i] = strdup(token);
                i++;
            }
        }

        token = strtok_r(NULL, " \t", &saveptr);
    }

    // Son komutun argümanlarını sonlandır
    if (cmd->pipe_count == 0)
    {
        cmd->args[i] = NULL;
        cmd->arg_count = i;
    }
    else
    {
        cmd->pipe_commands[current_pipe_cmd - 1][i] = NULL;
        cmd->pipe_arg_counts[current_pipe_cmd - 1] = i;
    }

    return 0;
}

void clear_command(Command *cmd)
{
    int i, j;

    // Ana komut argümanlarını temizle
    for (i = 0; i < cmd->arg_count; i++)
    {
        free(cmd->args[i]);
    }

    // Pipe komutlarını temizle
    for (i = 0; i < cmd->pipe_count; i++)
    {
        for (j = 0; j < cmd->pipe_arg_counts[i]; j++)
        {
            free(cmd->pipe_commands[i][j]);
        }
    }

    // Yönlendirme dosyalarını temizle
    if (cmd->input_file)
        free(cmd->input_file);
    if (cmd->output_file)
        free(cmd->output_file);

    // Yapıyı sıfırla
    memset(cmd, 0, sizeof(Command));
}
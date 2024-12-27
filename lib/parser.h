/**
 * Grup: [GRUP_NO]
 * Bu dosya Linux Shell uygulamasının ana program dosyasıdır.
 *
 * Yazanlar:
 * Samed Değer - B211210022
 * Saad Saad -B2112110552
 * FUAD ABDULLAH YAHYA AISHAN G201210562
 */

#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 64
#define MAX_PIPES 10

typedef struct
{
    char *args[MAX_ARGS];                     // Ana komut argümanları
    int arg_count;                            // Ana komut argüman sayısı
    char *pipe_commands[MAX_PIPES][MAX_ARGS]; // Pipe komutları
    int pipe_arg_counts[MAX_PIPES + 1];       // Her pipe komutu için argüman sayısı
    int pipe_count;                           // Pipe sayısı
    char *input_file;                         // Giriş yönlendirme dosyası
    char *output_file;                        // Çıkış yönlendirme dosyası
    int background;                           // Arkaplan işlem flag'i
} Command;

int parse_command(char *input, Command *cmd);
void clear_command(Command *cmd);

#endif
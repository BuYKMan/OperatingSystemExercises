/**
 * Grup: [GRUP_NO]
 * Proje büyüdüğünde daha esnek bir yapıya sahip olmamız için oluşturduğumuz bir sınıftır
 *
 * Yazanlar:
 * Samed Değer - B211210022
 * Saad Saad -B2112110552
 * FUAD ABDULLAH YAHYA AISHAN G201210562
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"

void error_exit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void *safe_malloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL)
    {
        error_exit("malloc");
    }
    return ptr;
}

int file_exists(const char *filename)
{
    return access(filename, F_OK) != -1;
}
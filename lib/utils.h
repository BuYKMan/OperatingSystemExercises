/**
 * Grup: [GRUP_NO]
 * Bu dosya Linux Shell uygulamasının ana program dosyasıdır.
 *
 * Yazanlar:
 * Samed Değer - B211210022
 * Saad Saad -B2112110552
 * FUAD ABDULLAH YAHYA AISHAN G201210562
 */

#ifndef UTILS_H
#define UTILS_H

void error_exit(const char *msg);
void *safe_malloc(size_t size);
int file_exists(const char *filename);

#endif // UTILS_H
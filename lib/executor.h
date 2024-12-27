/**
 * Grup: [GRUP_NO]
 * Bu dosya Linux Shell executor header dosyasıdır
 *
 * Yazanlar:
 * Samed Değer - B211210022
 * Saad Saad -B2112110552
 * FUAD ABDULLAH YAHYA AISHAN G201210562
 */
#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"

int execute_command(Command *cmd);
int execute_single_command(Command *cmd);
int execute_pipe_commands(Command *cmd);
int setup_redirections(Command *cmd);
int execute_built_in_command(Command *cmd);

#endif
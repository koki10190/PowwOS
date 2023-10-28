#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>

void render_shell_starter();
void cmd_handler(char *buf, bool terminal);

#endif
#include <shell/shell.h>
#include <string.h>
#include <display/display.h>
#include <graphics/graphics.h>
#include <time/rtc.h>
#include <stdlib.h>
#include <boot.h>
#include <app/import_apps.h>
#include <ramfs/fs.h>
#include <uart.h>

#define MATCH_BG_COLOR display_color == WHITE ? BLACK : WHITE

void render_shell_starter() {
    print("\r\n", WHITE);
    print(shell_name, shell_name_color);
    print(" ~ ", YELLOW);
}

char current_directory[500] = "/";

void __os_string_to_color(uint32_t *color, char *buffer, bool bg_mode) {
    if (!strcmp(buffer, "blue")) {
        (*color) = BLUE;
    }
    if (!strcmp(buffer, "lblue")) {
        (*color) = LIGHTBLUE;
    }
    if (!strcmp(buffer, "green")) {
        (*color) = GREEN;
    }
    if (!strcmp(buffer, "cyan")) {
        (*color) = CYAN;
    }
    if (!strcmp(buffer, "red")) {
        (*color) = RED;
    }
    if (!strcmp(buffer, "magenta")) {
        (*color) = MAGENTA;
    }
    if (!strcmp(buffer, "pink") || !strcmp(buffer, "lmagenta")) {
        (*color) = LIGHTMAGENTA;
    }
    if (!strcmp(buffer, "brown")) {
        (*color) = BROWN;
    }
    if (!strcmp(buffer, "lgray")) {
        (*color) = LIGHTGRAY;
    }
    if (!strcmp(buffer, "dgray")) {
        (*color) = DARKGRAY;
    }
    if (!strcmp(buffer, "lgreen")) {
        (*color) = LIGHTGREEN;
    }
    if (!strcmp(buffer, "lcyan")) {
        (*color) = LIGHTCYAN;
    }
    if (!strcmp(buffer, "lred")) {
        (*color) = LIGHTRED;
    }
    if (!strcmp(buffer, "yellow")) {
        (*color) = YELLOW;
    }
    if (!strcmp(buffer, "white")) {
        (*color) = WHITE;
    }
    if (!strcmp(buffer, "black")) {
        (*color) = BLACK;
    }
    if (!strcmp(buffer, "default")) {
        (*color) = 0x22bbdd;
    }
}

void cmd_print(char *buffer, uint32_t color, bool terminal) {
    if (terminal)
        terminal_print(buffer, color);
    else
        print(buffer, color);
}

void cmd_println(char *buffer, uint32_t color, bool terminal) {
    if (terminal)
        terminal_println(buffer, color);
    else
        println(buffer, color);
}

void cmd_handler(char *buffer, bool terminal) {
    char args[32][32];
    char *token;
    char *rest = buffer;
    int index = 0;

    while ((token = strtok_r(rest, " ", &rest))) {
        strcpy(args[index], token);
        index++;
    }

    if (!strcmp(args[0], "clear")) {
        if (terminal) {
            terminal_clear_screen();
            terminal_skip_enter_nl = true;
        } else {
            render_background(display_color);
            cursor_x = text_start_x;
            cursor_y = text_start_y;
            render_topbar();
        }
    } else if (!strcmp(args[0], "pwd")) {
        cmd_println("\r\n", WHITE, terminal);
        cmd_println(current_directory, WHITE, terminal);
    } else if (!strcmp(args[0], "cd")) {
        if (args[0][0] != '/') {
            strcat(current_directory, args[1]);
        } else {
            strcpy(current_directory, args[1]);
        }
    } else if (!strcmp(args[0], "touch")) {
        ramfs_create(args[1], (uint8_t *)"");
    } else if (!strcmp(args[0], "ls")) {
        cmd_println("\r\n", WHITE, terminal);
        for (int i = 0; i < MAX_RAMFS_FILES; i++) {
            if (!ramfs.files[i].deleted) {
                cmd_println(ramfs.files[i].path, WHITE, terminal);
                cmd_println("\r\n", WHITE, terminal);
            }
        }
    } else if (!strcmp(args[0], "cat")) {
        cmd_println("\r\n", WHITE, terminal);
        file_t *file = ramfs_open(args[1]);
        if (file) {
            cmd_println((char *)file->buffer, WHITE, terminal);
            cmd_println("\r\n", WHITE, terminal);
        } else {
            cmd_println("Invalid File!", LIGHTRED, terminal);
            cmd_println("\r\n", WHITE, terminal);
        }
    } else if (!strcmp(args[0], "write")) {
        cmd_println("\r\n", WHITE, false);
        file_t *file = ramfs_open(args[1]);
        file->buffer = (uint8_t *)args[2];
    } else if (!strcmp(args[0], "time")) {
        poww_time *date;
        get_time(date);
        cmd_println("\r\n", WHITE, terminal);
        cmd_print(__itoa(date->day), PINK, terminal);
        cmd_print("/", PINK, terminal);
        cmd_print(__itoa(date->month), PINK, terminal);
        cmd_print("/", PINK, terminal);
        cmd_print(__itoa(date->year), PINK, terminal);
        cmd_print(" ", PINK, terminal);

        cmd_print(__itoa(date->hour), PINK, terminal);
        cmd_print(":", PINK, terminal);
        cmd_println(__itoa(date->min), PINK, terminal);
    } else if (!strcmp(args[0], "set")) {
        if (!strcmp(args[1], "name")) {
            strcpy(shell_name, args[2]);
        }

        if (!strcmp(args[1], "name-color")) {
            __os_string_to_color(&shell_name_color, args[2], false);
        }

        if (!strcmp(args[1], "bg-color")) {
            __os_string_to_color(&display_color, args[2], false);
            render_background(display_color);
        }

    } else if (!strcmp(args[0], "rand")) {
        cmd_print("\r\n", WHITE, terminal);
        cmd_println(__itoa(rand() % 1000), MATCH_BG_COLOR, terminal);
    } else if (!strcmp(args[0], "panic")) {
        asm("int $0x0e");
    } else if (!strcmp(args[0], "calculator") || !strcmp(args[0], "calc")) {
        calculator_init();
    } else if (!strcmp(args[0], "terminal") || !strcmp(args[0], "shell") || !strcmp(args[0], "term")) {
        terminal_init();
    } else if (!strcmp(args[0], "osaka")) {
        osaka_init();
    } else {
        cmd_println("\r\n-> poww: Invalid Command!", LIGHTRED, terminal);
    }
}
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
#include <port_io.h>

#define MATCH_BG_COLOR display_color == WHITE ? BLACK : WHITE

// The poww ~ starter
void render_shell_starter() {
    print("\r\n", WHITE);
    print(shell_name, shell_name_color);
    print(" ~ ", YELLOW);
}

// Ignore thisd
char current_directory[500] = "/";

// Translate string to a hex color
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

// Terminal print
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

// Ignore this
static void play_sound(uint32_t nFrequence) {
    uint32_t Div;
    uint8_t tmp;

    // Set the PIT to the desired frequency
    Div = 1193180 / nFrequence;
    outb(0x43, 0xb6);
    outb(0x42, (uint8_t)(Div));
    outb(0x42, (uint8_t)(Div >> 8));

    // And play the sound using the PC speaker
    tmp = inb(0x61);
    if (tmp != (tmp | 3)) {
        outb(0x61, tmp | 3);
    }
}

// Ignore this
static void nosound() {
    uint8_t tmp = inb(0x61) & 0xFC;

    outb(0x61, tmp);
}

// Ignore this
void beep() {
    play_sound(1000);
    // nosound();
    // set_PIT_2(old_frequency);
}

// Command handler
void cmd_handler(char *buffer, bool terminal) {
    // Array of arguments
    char args[32][32];
    char *token;
    char *rest = buffer;
    int index = 0;

    // Split the string by a space and copy it to the args[index] string
    while ((token = strtok_r(rest, " ", &rest))) {
        strcpy(args[index], token);
        index++;
    }

    // Commands
    if (!strcmp(args[0], "clear")) {
        // clears the screen duh
        if (terminal) {
            terminal_clear_screen();
            terminal_skip_enter_nl = true;
        } else {
            render_background(display_color);
            cursor_x = text_start_x;
            cursor_y = text_start_y;
            render_topbar();
        }
    } else if (!strcmp(args[0], "touch")) {
        // Creates a "file" in the shitty RamFS
        char *path = args[1];
        ramfs_create(path, (uint8_t *)"");
    } else if (!strcmp(args[0], "ls")) {
        // Lists all "files" in the shitty RamFS
        cmd_println("\r\n", WHITE, terminal);
        for (int i = 0; i < MAX_RAMFS_FILES; i++) {
            if (!ramfs.files[i].deleted) {
                cmd_println(ramfs.files[i].path, WHITE, terminal);
                cmd_println("\r\n", WHITE, terminal);
                uart_puts(ramfs.files[i].path);
                uart_puts("\n");
            }
        }
    } else if (!strcmp(args[0], "beep")) {
        // Should beep but it doesn't cause i'm a fucking dumbass
        beep();
    } else if (!strcmp(args[0], "cat")) {
        // Print the text from a "file" in RamFS
        char *path = args[1];
        cmd_println("\r\n", WHITE, terminal);
        file_t *file = ramfs_open(path);
        if (file) {
            cmd_println((char *)file->buffer, WHITE, terminal);
            cmd_println("\r\n", WHITE, terminal);
        } else {
            cmd_println("Invalid File!", LIGHTRED, terminal);
            cmd_println("\r\n", WHITE, terminal);
        }
    } else if (!strcmp(args[0], "write")) {
        // Write to file
        char *path = args[1];
        cmd_println("\r\n", WHITE, false);
        file_t *file = ramfs_open(path);
        file->buffer = args[2];
    } else if (!strcmp(args[0], "time")) {
        // Get the full time and date and print it on the screen, DD/MM/YYYY HH:MM format
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
        // Set the user name
        if (!strcmp(args[1], "name")) {
            strcpy(shell_name, args[2]);
        }

        // Set the name color
        if (!strcmp(args[1], "name-color")) {
            __os_string_to_color(&shell_name_color, args[2], false);
        }

        // Set the background color, you can change this from an application too
        if (!strcmp(args[1], "bg-color")) {
            __os_string_to_color(&display_color, args[2], false);
            render_background(display_color);
        }
    } else if (!strcmp(args[0], "rand")) {
        // Print out a random number (for testing)
        cmd_print("\r\n", WHITE, terminal);
        cmd_println(__itoa(rand() % 1000), MATCH_BG_COLOR, terminal);
    } else if (!strcmp(args[0], "panic")) {
        // Invoke a Page Fault
        asm("int $0x0e");
    } else if (!strcmp(args[0], "calculator") || !strcmp(args[0], "calc")) {
        // Run calculator application
        calculator_init();
    } else if (!strcmp(args[0], "terminal") || !strcmp(args[0], "shell") || !strcmp(args[0], "term")) {
        // Run terminal application
        terminal_init();
    } else if (!strcmp(args[0], "osaka")) {
        // Run osaka application
        osaka_init();
    } else {
        // If none of the commands were found, then print out "Invalid Command"
        cmd_println("\r\n-> poww: Invalid Command!", LIGHTRED, terminal);
    }
}
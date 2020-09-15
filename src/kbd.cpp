#include "kbd.hpp"
#include <unistd.h>

Keyboard::Keyboard() {
    // Save original settings.
    tcgetattr(STDIN_FILENO, &init_settings);

    // Set up new settings.
    // Disable canonical mode and echo.
    new_settings = init_settings;
    new_settings.c_lflag &= ~(ICANON | ECHO); 
    // Set min characters for noncanonical read to 1 and timeout to be 0.
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;

    // Set new settings.
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);

    // Set the default peek value.
    peek_ch = -1;
}

Keyboard::~Keyboard() {
    // Restore original settings.
    tcsetattr(STDIN_FILENO, TCSANOW, &init_settings);
}

bool Keyboard::kbhit() {
    // If there is a new character on the buffer.
    if (peek_ch != -1) return true;

    // Set up noncanonical mode to accept reads of length 0.
    new_settings.c_cc[VMIN] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);

    // Try to read the next byte.
    char ch;
    int retval = read(STDIN_FILENO, &ch, 1);

    // Restore previous config.
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);

    // If a byte was able to be read.
    if (retval == 1) {
        peek_ch = ch;
        return true;
    }
    return false;
}

int Keyboard::getch() {
    char ch;

    // If the next character has been peeked at and exists.
    if (peek_ch != -1) {
        ch = peek_ch;
        peek_ch = -1;
    // Otherwise try to read the next character.
    } else {
        read(STDIN_FILENO, &ch, 1);
    }

    return ch;
}


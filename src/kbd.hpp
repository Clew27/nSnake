/** conio.h like implementation.
 * Based on termios.h and http://linux-sxs.org/programming/kbhit.html.
 */

#ifndef KBD_HPP
#define KBD_HPP

#include <sys/termios.h>

class Keyboard {
private:
    struct termios init_settings, new_settings;
    int peek_ch;

public:
    Keyboard();
    ~Keyboard();
    // Returns true if a key has been pressed.
    bool kbhit();
    // Returns the next character in the buffer. Will hang until a character is
    // available.
    int getch();
};

#endif /* KBD_HPP */


#ifndef ANSI_ESCSEQ_HPP
#define ANSI_ESCSEQ_HPP
#include <ostream>
#include <iostream>
#include <string>
#include <type_traits>

namespace ANSI {

enum Color {
    RESET        = 0,

    FG_DEFAULT   = 39,
    FG_BLACK     = 30,
    FG_RED       = 31,
    FG_GREEN     = 32,
    FG_YELLOW    = 33,
    FG_BLUE      = 34,
    FG_MAGENTA   = 35,
    FG_CYAN      = 36,
    FG_WHITE     = 37,
    FG_BBLACK    = 90,
    FG_BRED      = 91,
    FG_BGREEN    = 92,
    FG_BYELLOW   = 93,
    FG_BBLUE     = 94,
    FG_BMAGENTA  = 95,
    FG_BCYAN     = 96,
    FG_BWHITE    = 97,

    BG_DEFAULT   = 49,
    BG_BLACK     = 40,
    BG_RED       = 41,
    BG_GREEN     = 42,
    BG_YELLOW    = 43,
    BG_BLUE      = 44,
    BG_MAGENTA   = 45,
    BG_CYAN      = 46,
    BG_WHITE     = 47,
    BG_BBLACK    = 100,
    BG_BRED      = 101,
    BG_BGREEN    = 102,
    BG_BYELLOW   = 103,
    BG_BBLUE     = 104,
    BG_BMAGENTA  = 105,
    BG_BCYAN     = 106,
    BG_BWHITE    = 107,
};

enum Movement {
    CURSOR_UP    = 'A',
    CURSOR_DOWN  = 'B',
    CURSOR_RIGHT = 'C',
    CURSOR_LEFT  = 'D'
};

enum Display {
    ERASE_DISPLAY = 'J',
    ERASE_LINE    = 'K'
};

// ANSI escape sequence superclass.
class Modifier {
protected:
    std::string escseq;
public:
    Modifier(std::string escseq_) : escseq(escseq_) {}
    friend std::ostream& operator<<(std::ostream& os, const Modifier& mod) {
        return os << "\033[" << mod.escseq;
    }
};

// Modifier for FG/BG colors.
class ColorModifier : public Modifier {
public:
    ColorModifier(Color code) : Modifier("") {
        escseq = std::to_string(code) + "m";
    }
};

// Modifier for relative and absolute movements.
class MovementModifier : public Modifier {
public:
    // Relative movement in direction. 
    MovementModifier(int dist, Movement dir) : Modifier("") {
        escseq = std::to_string(dist) + static_cast<char>(dir);
    }
    // Absolute movement in line and column numbers.
    MovementModifier(int line, int col) : Modifier("") {
        escseq = std::to_string(line) + ";" + std::to_string(col) + "H";
    }
};

// Modifier for the display (currently erase).
class DisplayModifier : public Modifier {
public:
    // Erase modes that could start from origin or the cursor.
    DisplayModifier(Display type, bool from_cursor = false) : Modifier("") {
        escseq = ((from_cursor) ? "" : "2");
        escseq += static_cast<char>(type);
    }
};

//class UnionModifier : public Modifier {
//public:
    //template <class T,
    //UnionModifier(
//};

}

#endif /* ANSI_ESCSEQ_HPP */


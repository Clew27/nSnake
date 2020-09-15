#ifndef GAME_HPP
#define GAME_HPP

// Define the height of the score tab. As a precompiler flag this allows for
// constant squashing later in the code.
#define SCORE_TAB_HEIGHT 1

#include <deque>
#include "kbd.hpp"
#include "ansi_escseq.hpp"

/** Coordinate system (x, y)
 * Top left is (0, 0)
 * x = col #
 * y = line #
 */

enum Collision {
    APPLE, SNAKE, OUT_OF_BOUNDS, NONE
};

class Game {
private:
    // Dimensions of playing board.
    int game_width;
    int game_height;

    // Game speed (in seconds).
    double tick_rate = 0.1;
    // Game time.
    int ticks = 0;
    int old_ticks = 0;
    // Game score.
    int score = 0;

    // I/O.
    Keyboard kbd;

    // Game elements.
    std::deque<std::pair<int, int>> snake;
    std::pair<int, int> apple;
    std::pair<int, int> heading;

    // Positions for console.
    ANSI::MovementModifier home;
    ANSI::MovementModifier reset;

    // Renders the frame from current cursor position.
    void render_main_frame();
    // Render the score frame.
    void render_score_frame();
    // Render the score.
    void render_score();
    // Render the time.
    void render_time();
    // Render the whole snake.
    void render_snake();
    // Render the snake (depends on if the snake length increases).
    void render_snake_update(bool consumed = false);
    // Render the apple.
    void render_apple(std::pair<int, int> new_apple);
    // Clear screen and homes cursor.
    void render_cls();

    // Moves to x, y coordinates on the game board (adjusted for offsets created
    // by score tab).
    void move_to(std::pair<int, int> pos);

    // Initialize starting snake.
    void initialize_game();

    // Updates the direction of the snake.
    void update_direction();
    // Updates the snake.
    bool update_snake();
    // Updates the apple's location.
    void update_apple();
    // Update score.
    void update_score();

    // Returns the type of collision if any.
    Collision check_collision(std::pair<int, int> pos);

    // Runs one cycle of the game and returns whether the game is going to continue.
    bool game_cycle();


public:
    Game();
    Game(int game_width_, int game_height_);
    void start();
};

#endif /* GAME_HPP */


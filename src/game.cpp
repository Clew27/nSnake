#include "game.hpp"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>

using tp_t = std::chrono::time_point<std::chrono::high_resolution_clock>;
// Wait for a tick_rate amount of time given starting time.
tp_t wait_till(tp_t start, double tick_rate) {
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = now - start;
    while (diff.count() < tick_rate) {
        now = std::chrono::high_resolution_clock::now();
        diff = now - start;
    }
    return now;
}

//Game::Game() : Game(100, 25) {
Game::Game() : Game(50, 25) {
}

Game::Game(int game_width_, int game_height_) 
    : game_width(game_width_)
    , game_height(game_height_)
    , heading(std::make_pair(1, 0))
    , home(1, 1)
    , reset(game_height + SCORE_TAB_HEIGHT + 3, 1)
{}

void Game::render_main_frame() {
    // Render top frame.
    std::cout << '+';
    for (int i = 0; i < game_width; ++i) std::cout << '-';
    std::cout << '+' << std::endl;

    // Render two side frames.
    // Will increase the height by 2 for the score bar in-game.
    for (int i = 0; i < game_height + (SCORE_TAB_HEIGHT + 1); ++i) {
        std::cout << '|';
        for (int j = 0; j < game_width; ++j) std::cout << ' ';
        std::cout << '|' << std::endl;
    }

    // Render bottom frame.
    std::cout << '+';
    for (int i = 0; i < game_width; ++i) std::cout << '-';
    std::cout << '+';
}

void Game::render_score_frame() {
    std::cout << ANSI::MovementModifier(1 + SCORE_TAB_HEIGHT + 1, 1);
    std::cout << '+';
    for (int i = 0; i < game_width; ++i) std::cout << '-';
    std::cout << '+';
}

void Game::render_score() {
    std::cout << ANSI::MovementModifier(2, 2);
    std::cout << "Score: " << score;
}

void Game::render_time() {
    double actual = ticks * tick_rate;

    int len = std::snprintf(nullptr, 0, "%0.1f", actual);
    std::cout << ANSI::MovementModifier(2, 2 + game_width - len); 
    std::printf("%0.1f", actual);
}

void Game::render_snake() {
    // Draw the body.
    for (auto pos : snake) {
        move_to(pos);
        std::cout << 'o';
    }

    // Draw the head.
    move_to(snake.front());
    std::cout << 'O';
}

void Game::render_snake_update(bool consumed) {
    // Draw the new head.
    auto& head = snake.front();
    move_to(head);
    std::cout << 'O';
    auto& old_head = snake[1];
    move_to(old_head);
    std::cout << 'o';

    // Remove the tail if necessary. 
    if (!consumed) {
        auto& old_tail = snake.back();        
        move_to(old_tail);
        std::cout << ' ';
        snake.pop_back();
    }
}

void Game::render_apple(std::pair<int, int> new_apple) {
    // Delete old apple.
    move_to(apple);
    std::cout << ' ';
    
    // Draw new apple.
    move_to(new_apple);
    std::cout << '+';

    // Save the new position.
    apple = new_apple;
}

void Game::render_cls() {
    std::cout << ANSI::DisplayModifier(ANSI::ERASE_DISPLAY);
    std::cout << home; 
}

void Game::move_to(std::pair<int, int> pos) {
    int x = pos.first, y = pos.second;
    
    // Line number and columns are 1-indexed so there's an automatic +1.
    // Furthermore, for the height the top border, the score section, and the
    // score section's bottom border needs to be accounted for.
    std::cout << ANSI::MovementModifier(y + 2 + SCORE_TAB_HEIGHT + 1,
            x + 2);
}

void Game::initialize_game() {
    // Reset values.
    score = 0;
    ticks = 0;
    old_ticks = 0;

    // Set new snake.
    snake.clear();
    for (int i = 0; i < 3; ++i) 
        snake.push_back(std::make_pair(game_width / 2 - i, game_height / 2));

    // Render elements.
    render_cls();
    render_main_frame();
    render_score_frame();
    render_score();
    render_time();
    render_snake();
    update_apple();
    std::cout << reset << std::endl;
}


void Game::update_direction() {
    // Need to maintain old heading to cover the case when multiple user inputs
    // within the same tick causes the heading to be 180'd. Suppose heanding is
    // left and the user inputs bottom, right. If old_heading is not preserved,
    // the heading will be changed to bottom then to right which seems to be 
    // valid but not.
    auto old_heading = heading;
    // Get all updates available in the buffer.
    while (kbd.kbhit()) {
        // If it's not an arrow key input skip.
        if (kbd.getch() != '\033') continue;
        // Otherwise get the direction and update.
        // Don't change direction if it flips the previous heading (snake
        // can't travel inwards into itself).
        kbd.getch();
        switch(kbd.getch()) {
            case 'A':
                if (old_heading.second != 1) heading = {0, -1};
                break;
            case 'B':
                if (old_heading.second != -1) heading = {0, 1};
                break;
            case 'C':
                if (old_heading.first != -1) heading = {1, 0};
                break;
            case 'D':
                if (old_heading.first != 1) heading = {-1, 0};
                break;
        }
    }
}

bool Game::update_snake() {
    // Compute the snake's new head.
    auto& head = snake.front();
    std::pair<int, int> new_head 
        {head.first + heading.first, head.second + heading.second};

    // Check the state of the snake.
    bool consumed = false;
    switch(check_collision(new_head)) {
        case APPLE:
            snake.push_front(new_head);
            update_apple();
            update_score();
            consumed = true;
            break;
        case NONE:
            snake.push_front(new_head);
            break;
        case SNAKE:
        case OUT_OF_BOUNDS:
            return false;
    }

    // Render the snake's new position.
    render_snake_update(consumed);

    return true;
}

void Game::update_apple() {
    // Compute new apple position.
    std::pair<int, int> new_apple;
    do {
        new_apple = {rand() % game_width, rand() % game_height};
    } while (check_collision(new_apple) == Collision::SNAKE);
    
    // Render the apple's new position.
    render_apple(new_apple);
}

void Game::update_score() {
    // Compute the score.
    // TODO: Change scoring to reward speed. 
    score += snake.size();

    // Set ticks.
    old_ticks = ticks;

    // Render the score.
    render_score();
}

Collision Game::check_collision(std::pair<int, int> pos) {
    int x = pos.first, y = pos.second;

    // If out of bounds.
    if (x < 0 || x >= game_width || y < 0 || y >= game_height)
        return Collision::OUT_OF_BOUNDS;
    
    // If collided with apple.
    if (pos == apple) return Collision::APPLE;

    // Check if collided with snake.
    // TODO: Convert the operation to O(1) with a set.
    for (auto spos : snake) {
        if (pos == spos) return Collision::SNAKE;
    }

    return Collision::NONE;
}

bool Game::game_cycle() {
    update_direction();
    bool res = update_snake();
    render_time();

    std::cout << reset << std::endl;
    return res;
}

void Game::start() {
    // Set random seed.
    srand(time(NULL));

    // Initialize game variables.
    initialize_game();

    // Run game until fail/success.
    auto start = std::chrono::high_resolution_clock::now();
    for (;;) {
        // Wait until the next tick begins.
        start = wait_till(start, tick_rate);
        
        // If the game is over, break out of the loop.
        if (!game_cycle()) break;

        // Otherwise, increase the tick count and continue.
        ++ticks;
    }
}


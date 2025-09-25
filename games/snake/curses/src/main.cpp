#include "game.h"
#include "terminal.h"
#include "render.h"
#include <iostream>
#include <thread>

int main() {
    Terminal term;
    int choice = promptDifficulty();
    term.enableRawMode();

    while (true) {
        int width, height;
        term.getWindowSize(width, height);
        int game_width = (width / 2) - 2;
        int game_height = height - 3;

        if (game_width < 10 || game_height < 5) {
            std::cerr << "Terminal too small. Resize a bit.\n";
            return 1;
        }

        Game game(game_width, game_height, choice);
        Renderer renderer(game_width, game_height);
        Input input;

        while (!game.isGameOver()) {
            if (Terminal::isResized()) {
                Terminal::setResized(false);
                term.getWindowSize(width, height);
                game_width = (width / 2) - 2;
                game_height = height - 3;
                game.resize(game_width, game_height);
                renderer.resize(game_width, game_height);
            }

            game.processInput(&input);
            game.update();
            renderer.draw(game.getSnake(), game.getFood(), game.getScore(), game.getHighscore());
            if(game.isPaused()) {
                renderer.drawPaused();
            }
        }

        if (game.getScore() > loadHighscore()) {
            saveHighscore(game.getScore());
        }
        renderer.drawGameOver(game.getScore());

        bool quit = false;
        while (!quit) {
            UserAction action = input.getUserAction(RIGHT); // direction doesn't matter here
            switch (action) {
                case UserAction::QUIT:
                    quit = true;
                    break;
                case UserAction::RESTART:
                    goto next_game;
                case UserAction::CHANGE_DIFFICULTY:
                    term.disableRawMode();
                    choice = promptDifficulty();
                    term.enableRawMode();
                    goto next_game;
                default:
                    break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        if (quit) break;

    next_game:;
    }

    std::cout << "\nThanks for playing.\n";
    return 0;
}

#include <gtest/gtest.h>
#include "game.h"
#include "input.h"

TEST(GameTest, Initialization) {
    Game game(20, 10, 0);
    ASSERT_EQ(game.getScore(), 0);
    ASSERT_FALSE(game.isGameOver());
}

TEST(GameTest, SnakeMovement) {
    Game game(20, 10, 0, {{10, 5}, {9, 5}, {8, 5}}, {15, 15});
    
    game.update(); // Moves RIGHT by default
    auto snake = game.getSnake();
    EXPECT_EQ(snake.front().x, 11);
    EXPECT_EQ(snake.front().y, 5);
    EXPECT_EQ(snake.size(), 3);
}

TEST(GameTest, WallCollision) {
    Game game(20, 10, 0, {{19, 5}, {18, 5}, {17, 5}}, {15, 15});
    game.update(); // Moves RIGHT into the wall
    EXPECT_TRUE(game.isGameOver());
}

TEST(GameTest, SelfCollision) {
    // Snake shape:
    //
    //   H B B B
    //         B
    Game game(20, 10, 0, {{10, 5}, {10, 6}, {10, 7}, {10, 8}, {11, 8}}, {15, 15});
    game.setDirection(UP);
    game.update(); // Up
    game.setDirection(LEFT);
    game.update(); // Left
    game.setDirection(DOWN);
    game.update(); // Down, collides with {10, 6}
    EXPECT_TRUE(game.isGameOver());
}

TEST(GameTest, FoodConsumption) {
    Game game(20, 10, 0, {{10, 5}, {9, 5}, {8, 5}}, {11, 5});
    EXPECT_EQ(game.getScore(), 0);
    EXPECT_EQ(game.getSnake().size(), 3);

    game.update(); // Moves RIGHT and eats the food

    EXPECT_EQ(game.getScore(), 1);
    EXPECT_EQ(game.getSnake().size(), 4);
    EXPECT_NE(game.getFood().x, 11); // Food should have moved
    EXPECT_NE(game.getFood().y, 5);
}

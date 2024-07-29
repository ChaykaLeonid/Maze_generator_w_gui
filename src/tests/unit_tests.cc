#include <gtest/gtest.h>

#include "../Maze/model.h"

class MazeTest : public testing::Test {
 protected:
  s21::Model maze;
  int errors = 0;
};

TEST_F(MazeTest, Gen) {
  room start(0, 0);
  room finish(10, 10);
  std::vector<room> path;

  s21::Model maze;
  int errors = 0;

  for (int i = 0; i < 1000; ++i) {
    maze.GenerateMaze(10, 10);
    if (maze.ContainsErrors()) errors++;
    maze.FindPath(start, finish, path);
    path.clear();
  }
  EXPECT_EQ(errors, 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
#include <gtest/gtest.h>

#include "TetrisFactory.hpp"
#include "Tetromino.hpp"


TEST(TetrisFactoryTest, Constructor) {
    TetrisFactory factory = TetrisFactory();
    EXPECT_FALSE(factory.isPoolEmpty()) << "The pool should not be empty after construction.";
    EXPECT_EQ(factory.getPoolSize(), 7) << "The pool should contain 7 pieces after construction.";
}


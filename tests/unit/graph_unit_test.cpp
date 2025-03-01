#include <gtest/gtest.h>

TEST(Graph_main, test1) {
    ASSERT_EQ(1, 1);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
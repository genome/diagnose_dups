#include "common/Histogram.hpp"

#include <gtest/gtest.h>

TEST(TestHistogram, merge) {
    Histogram<int> x;
    Histogram<int> y;

    x[1] = 4;
    x[2] = 5;
    y[3] = 6;
    y[4] = 7;

    Histogram<int> expected;
    expected[1] = 4;
    expected[2] = 5;
    expected[3] = 6;
    expected[4] = 7;

    x.merge(y);
    EXPECT_EQ(expected, x);
    EXPECT_EQ(2u, y.size());
}

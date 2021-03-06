#include "test.h"

#include "fay/math/math.h"

TEST(math2, min_max)
{
    //static_assert(-2 == fay::min(-2, -1, 0, 1, 2), "fay::min is failure");
    ASSERT_EQ(-2, fay::min(-2, -1, 0, 1, 2));
    //ASSERT_EQ(-2, compile_value<fay::min(-2, -1, 0, 1, 2)>());

    //static_assert(2 == fay::max(-2, -1, 0, 1, 2), "fay::max is failure");
    ASSERT_EQ(2, fay::max(-2, -1, 0, 1, 2));
    //ASSERT_EQ(2, compile_value<fay::max(-2, -1, 0, 1, 2)>());
}

TEST(math2, equal)
{
    ASSERT_EQ(false, fay::is_equal(0.f, 0.1f));
    ASSERT_EQ(true, fay::not_equal(0.f, 0.1f));

    ASSERT_EQ(true,   fay::is_equal<int>(0.f, 0.1f));
    ASSERT_EQ(false, fay::not_equal<int>(0.f, 0.1f));
}

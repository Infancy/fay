#include "test.h"

#include "fay/core/enum.h"
#include "fay/core/fay.h"

using namespace std;
using namespace fay;

// -------------------------------------------------------------------------------------------------

struct number
{
    enum enum_type
    {
        zero = 0b000,
        one = 0b001,
        two = 0b010,
        three = 0b011
    };
    FAY_ENUM_METHODS(number)

        constexpr bool is_one() const { return e_ == enum_type::one; }
};

template<typename E>
constexpr bool is_scoped_enum()
{
    static_assert(std::is_enum_v<E>, "E must be enum or enum class");

    if constexpr(std::is_convertible_v<E, std::underlying_type_t<std::decay_t<E>>>)
        return true;
    else
        return false;
}

// TODO: namespace
/*
struct number
{
    enum enum_type
    {
        none  = 0,
        zero  = 0b000,
        one   = 0b001,
        two   = 0b010,
        three = 0b011
    };
};
*/

TEST(core, enum_)
{
    //constexpr number one = number::one;

    constexpr number one{ number::one };
    constexpr number two{ number::two };
    const auto zero  = one & two;
    const auto three = one | two;

    constexpr auto enum_one = number::one;
    static_assert(!std::is_same_v<decltype(one), decltype(enum_one)>, "type of one and enum_one isn't same");

    static_assert(one.is_one(), "");
    ASSERT_TRUE(compile_value<one.is_one()>());
    ASSERT_TRUE(one.is_one());

    ASSERT_TRUE(zero  == number{ number::zero  });
    ASSERT_TRUE(three == number{ number::three });
}
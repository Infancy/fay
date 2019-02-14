#include <gtest/gtest.h>

#include "fay/core/fay.h"
#include "fay/core/hash.h"

using namespace std;
using namespace fay;

// -------------------------------------------------------------------------------------------------

template<auto hash>
uint64_t compile_hash() { return hash; }

TEST(hash, fnv_hash)
{
    using fnv = fnv<uint64_t>;
    //auto h1 = compile_hash < (0xcbf29ce484222325ull ^ uint64_t('F')) * 0x100000001b3ull>();
    //auto hash = compile_hash<fnv::hash("F")>();
    static_assert(fnv::hash("F") == 0xaf63fb4c86022139, "fnv1a_64::hash failure");
    static_assert(fnv::hash("FN") == 0x09057a07b59edd35, "fnv1a_64::hash failure");
    static_assert(fnv::hash("FNV") == 0xf32bbe199cf22f39, "fnv1a_64::hash failure");
    static_assert(fnv::hash("FNV ") == 0x25831e85af86077b, "fnv1a_64::hash failure");
    static_assert(fnv::hash("FNV H") == 0x43d4102940be3ba9, "fnv1a_64::hash failure");
    static_assert(fnv::hash("FNV Ha") == 0xff933e19033f94d8, "fnv1a_64::hash failure");
    static_assert(fnv::hash("FNV Has") == 0x86c72f8085099e91, "fnv1a_64::hash failure");
    static_assert(fnv::hash("FNV Hash") == 0x0e14b0620f59211b, "fnv1a_64::hash failure");
    static_assert(fnv::hash("FNV Hash ") == 0x4648f1a014737741, "fnv1a_64::hash failure");
    static_assert(fnv::hash("FNV Hash T") == 0xe169a802c03358af, "fnv1a_64::hash failure");
    static_assert(fnv::hash("FNV Hash Te") == 0x39e146ac973fdf3e, "fnv1a_64::hash failure");
    static_assert(fnv::hash("FNV Hash Tes") == 0x99aa644501886fd7, "fnv1a_64::hash failure");
    static_assert(fnv::hash("FNV Hash Test") == 0xa4f804419ad5b1f9, "fnv1a_64::hash failure");

    static_assert(fnv::hash("FNV Hash Test") == fnv::hash("FNV Hash Test"), "fnv1a_64::hash failure");

    //cout << fnv::hash("FNV Hash Test") << '\n';
    //cout << compile_hash<fnv::hash("FNV Hash Test")>() << '\n';

    ASSERT_TRUE(fnv::hash("F") == 0xaf63fb4c86022139);
    ASSERT_TRUE(fnv::hash("FN") == 0x09057a07b59edd35);
    ASSERT_TRUE(fnv::hash("FNV") == 0xf32bbe199cf22f39);
    ASSERT_TRUE(fnv::hash("FNV ") == 0x25831e85af86077b);
    ASSERT_TRUE(fnv::hash("FNV H") == 0x43d4102940be3ba9);
    ASSERT_TRUE(fnv::hash("FNV Ha") == 0xff933e19033f94d8);
    ASSERT_TRUE(fnv::hash("FNV Has") == 0x86c72f8085099e91);
    ASSERT_TRUE(fnv::hash("FNV Hash") == 0x0e14b0620f59211b);
    ASSERT_TRUE(fnv::hash("FNV Hash ") == 0x4648f1a014737741);
    ASSERT_TRUE(fnv::hash("FNV Hash T") == 0xe169a802c03358af);
    ASSERT_TRUE(fnv::hash("FNV Hash Te") == 0x39e146ac973fdf3e);
    ASSERT_TRUE(fnv::hash("FNV Hash Tes") == 0x99aa644501886fd7);
    ASSERT_TRUE(fnv::hash("FNV Hash Test") == 0xa4f804419ad5b1f9);

    ASSERT_TRUE(fnv::hash("F") == compile_hash<fnv::hash("F")>());
    ASSERT_TRUE(fnv::hash("FN") == compile_hash<fnv::hash("FN")>());
    ASSERT_TRUE(fnv::hash("FNV") == compile_hash<fnv::hash("FNV")>());
    ASSERT_TRUE(fnv::hash("FNV ") == compile_hash<fnv::hash("FNV ")>());
    ASSERT_TRUE(fnv::hash("FNV H") == compile_hash<fnv::hash("FNV H")>());
    ASSERT_TRUE(fnv::hash("FNV Ha") == compile_hash<fnv::hash("FNV Ha")>());
    ASSERT_TRUE(fnv::hash("FNV Has") == compile_hash<fnv::hash("FNV Has")>());
    ASSERT_TRUE(fnv::hash("FNV Hash") == compile_hash<fnv::hash("FNV Hash")>());
    ASSERT_TRUE(fnv::hash("FNV Hash ") == compile_hash<fnv::hash("FNV Hash ")>());
    ASSERT_TRUE(fnv::hash("FNV Hash T") == compile_hash<fnv::hash("FNV Hash T")>());
    ASSERT_TRUE(fnv::hash("FNV Hash Te") == compile_hash<fnv::hash("FNV Hash Te")>());
    ASSERT_TRUE(fnv::hash("FNV Hash Tes") == compile_hash<fnv::hash("FNV Hash Tes")>());
    ASSERT_TRUE(fnv::hash("FNV Hash Test") == compile_hash<fnv::hash("FNV Hash Test")>());
}

TEST(hash, fnv_merge)
{
    using fnv = fnv<uint64_t>;

    char merge_str[] = "FNV Hash";
    uint64_t merge_a = *(reinterpret_cast<uint64_t*>(merge_str));

    /*
    cout 
        << uint8_t(a >> 56)
        << uint8_t(a >> 48)
        << uint8_t(a >> 40)
        << uint8_t(a >> 32)
        << uint8_t(a >> 24)
        << uint8_t(a >> 16)
        << uint8_t(a >> 8)
        << uint8_t(a >> 0) << '\n';
    */

    ASSERT_TRUE(fnv::hash(merge_str) == fnv::merge(merge_a, 0xcbf29ce484222325ull));
}
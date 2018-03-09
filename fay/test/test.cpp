#include <gtest/gtest.h>

GTEST_API_ int main(int argc, char **argv)
{
	printf("Running main() from test/test.cpp\n");
	testing::InitGoogleTest(&argc, argv);
	auto result = RUN_ALL_TESTS();
	getchar();
	return result;
}
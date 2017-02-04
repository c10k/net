#include "net.hpp"
#include <gtest/gtest.h>

TEST(saysHello, returnCorrectString)
{

	std::string s("Hello");
	ASSERT_EQ(s, net::sayHello());
}

#include "catch.hpp"
#include <cassert>
#include "tim/result/Result.hpp"

TEST_CASE("Result Status") {
	{
		using V = tim::Result<int, long>;
		constexpr V v;
		static_assert(v.has_value(), "");
	}
	{
		using V = tim::Result<int, long>;
		V v;
		REQUIRE(v.has_value());
	}
	{
		using V = tim::Result<int, long>;
		constexpr V v(tim::in_place_error);
		static_assert(!v.has_value(), "");
	}
	{
		using V = tim::Result<int, std::string>;
		V v(tim::Error("abc"));
		REQUIRE(!v.has_value());
		v = 42;
		REQUIRE(v.has_value());
	}
}

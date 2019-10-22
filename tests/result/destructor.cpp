#include "catch.hpp"
#include "tim/result/Result.hpp"

struct NonTDtor {
	static int count;
	NonTDtor() = default;
	~NonTDtor() { ++count; }
};
int NonTDtor::count = 0;
static_assert(!std::is_trivially_destructible<NonTDtor>::value, "");

struct NonTDtor1 {
	static int count;
	NonTDtor1() = default;
	~NonTDtor1() { ++count; }
};
int NonTDtor1::count = 0;
static_assert(!std::is_trivially_destructible<NonTDtor1>::value, "");

struct TDtor {
	TDtor(const TDtor &) {} // non-trivial copy
	~TDtor() = default;
};
static_assert(!std::is_trivially_copy_constructible<TDtor>::value, "");
static_assert(std::is_trivially_destructible<TDtor>::value, "");

TEST_CASE("Result Destructor") {
	{
		using V = tim::Result<int, TDtor>;
		static_assert(std::is_trivially_destructible<V>::value, "");
	}
	{
		using V = tim::Result<NonTDtor, NonTDtor1>;
		static_assert(!std::is_trivially_destructible<V>::value, "");
		V v(tim::in_place);
		REQUIRE(NonTDtor::count == 0);
		REQUIRE(NonTDtor1::count == 0);
	}
	REQUIRE(NonTDtor::count == 1);
	REQUIRE(NonTDtor1::count == 0);
	NonTDtor::count = 0;

	{
		using V = tim::Result<NonTDtor, NonTDtor1>;
		static_assert(!std::is_trivially_destructible<V>::value, "");
		V v(tim::in_place_error);
		REQUIRE(NonTDtor::count == 0);
		REQUIRE(NonTDtor1::count == 0);
	}
	REQUIRE(NonTDtor::count == 0);
	REQUIRE(NonTDtor1::count == 1);
	NonTDtor1::count = 0;


	{
		using V = tim::Result<int, NonTDtor1>;
		V v(tim::in_place);
	}
	REQUIRE(NonTDtor1::count == 0);

	{
		using V = tim::Result<NonTDtor1, int>;
		V v(tim::in_place);
	}
	REQUIRE(NonTDtor1::count == 1);
	NonTDtor1::count = 0;

	{
		using V = tim::Result<int, NonTDtor1>;
		V v(tim::in_place_error);
	}
	REQUIRE(NonTDtor1::count == 1);
	NonTDtor1::count = 0;

	{
		using V = tim::Result<NonTDtor1, int>;
		V v(tim::in_place_error);
	}
	REQUIRE(NonTDtor1::count == 0);

	{
		using V = tim::Result<void, NonTDtor>;
		V v(tim::in_place_error);
	}
	REQUIRE(NonTDtor::count == 1);
	NonTDtor::count = 0;

	{
		using V = tim::Result<void, NonTDtor>;
		V v(tim::in_place);
	}
	REQUIRE(NonTDtor::count == 0);
	NonTDtor::count = 0;
}

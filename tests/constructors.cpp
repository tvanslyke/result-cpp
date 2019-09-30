#include "catch.hpp"
#include "tim/result/Result.hpp"

#include <vector>
#include <type_traits>
#include <string>

struct takes_init_and_variadic {
    std::vector<int> v;
    std::tuple<int, int> t;
    template <class... Args>
    takes_init_and_variadic(std::initializer_list<int> l, Args &&... args)
        : v(l), t(std::forward<Args>(args)...) {}
};

TEST_CASE("Constructors", "[constructors]") {
    {
        tim::Result<int,int> e;
        REQUIRE(e);
        REQUIRE(e == 0);
    }

    {
        tim::Result<int,int> e = tim::make_error(0);
        REQUIRE(!e);
        REQUIRE(e.error() == 0);
    }

    {
        tim::Result<int,int> e (tim::in_place_error, 0);
        REQUIRE(!e);
        REQUIRE(e.error() == 0);
    }

    {
        tim::Result<int,int> e (tim::in_place, 42);
        REQUIRE(e);
        REQUIRE(e == 42);
    }

    {
        tim::Result<std::vector<int>,int> e (tim::in_place, {0,1});
        REQUIRE(e);
        REQUIRE((*e)[0] == 0);
        REQUIRE((*e)[1] == 1);
    }

    {
        tim::Result<std::tuple<int,int>,int> e (tim::in_place, 0, 1);
        REQUIRE(e);
        REQUIRE(std::get<0>(*e) == 0);
        REQUIRE(std::get<1>(*e) == 1);
    }

    {
        tim::Result<takes_init_and_variadic,int> e (tim::in_place, {0,1}, 2, 3);
        REQUIRE(e);
        REQUIRE(e->v[0] == 0);
        REQUIRE(e->v[1] == 1);
        REQUIRE(std::get<0>(e->t) == 2);
        REQUIRE(std::get<1>(e->t) == 3);
    }

	{
		tim::Result<int, int> e;
		REQUIRE(std::is_default_constructible<decltype(e)>::value);
		REQUIRE(std::is_copy_constructible<decltype(e)>::value);
		REQUIRE(std::is_move_constructible<decltype(e)>::value);
		REQUIRE(std::is_copy_assignable<decltype(e)>::value);
		REQUIRE(std::is_move_assignable<decltype(e)>::value);
		REQUIRE(std::is_trivially_copy_constructible<decltype(e)>::value);
		REQUIRE(std::is_trivially_copy_assignable<decltype(e)>::value);
		REQUIRE(std::is_trivially_move_constructible<decltype(e)>::value);
		REQUIRE(std::is_trivially_move_assignable<decltype(e)>::value);
	}

	{
		tim::Result<int, std::string> e;
		REQUIRE(std::is_default_constructible<decltype(e)>::value);
		REQUIRE(std::is_copy_constructible<decltype(e)>::value);
		REQUIRE(std::is_move_constructible<decltype(e)>::value);
		REQUIRE(std::is_copy_assignable<decltype(e)>::value);
		REQUIRE(std::is_move_assignable<decltype(e)>::value);
		REQUIRE(!std::is_trivially_copy_constructible<decltype(e)>::value);
		REQUIRE(!std::is_trivially_copy_assignable<decltype(e)>::value);
		REQUIRE(!std::is_trivially_move_constructible<decltype(e)>::value);
		REQUIRE(!std::is_trivially_move_assignable<decltype(e)>::value);
	}

	{
		tim::Result<std::string, int> e;
		REQUIRE(std::is_default_constructible<decltype(e)>::value);
		REQUIRE(std::is_copy_constructible<decltype(e)>::value);
		REQUIRE(std::is_move_constructible<decltype(e)>::value);
		REQUIRE(std::is_copy_assignable<decltype(e)>::value);
		REQUIRE(std::is_move_assignable<decltype(e)>::value);
		REQUIRE(!std::is_trivially_copy_constructible<decltype(e)>::value);
		REQUIRE(!std::is_trivially_copy_assignable<decltype(e)>::value);
		REQUIRE(!std::is_trivially_move_constructible<decltype(e)>::value);
		REQUIRE(!std::is_trivially_move_assignable<decltype(e)>::value);
	}

	{
		tim::Result<std::string, std::string> e;
		REQUIRE(std::is_default_constructible<decltype(e)>::value);
		REQUIRE(std::is_copy_constructible<decltype(e)>::value);
		REQUIRE(std::is_move_constructible<decltype(e)>::value);
		REQUIRE(std::is_copy_assignable<decltype(e)>::value);
		REQUIRE(std::is_move_assignable<decltype(e)>::value);
		REQUIRE(!std::is_trivially_copy_constructible<decltype(e)>::value);
		REQUIRE(!std::is_trivially_copy_assignable<decltype(e)>::value);
		REQUIRE(!std::is_trivially_move_constructible<decltype(e)>::value);
		REQUIRE(!std::is_trivially_move_assignable<decltype(e)>::value);
	}

    {
        tim::Result<void,int> e;
        REQUIRE(e);
    }

    {
        tim::Result<void,int> e (tim::in_place_error, 42);
        REQUIRE(!e);
        REQUIRE(e.error() == 42);
    }
}

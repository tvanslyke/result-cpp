#include "catch.hpp"
#include "tim/result/Result.hpp"

#include <string>

// Old versions of GCC don't have the correct trait names. Could fix them up if needs be.
#if (defined(__GNUC__) && __GNUC__ == 4 && __GNUC_MINOR__ <= 9 &&              \
     !defined(__clang__))
// nothing for now
#else
TEST_CASE("Triviality", "[bases.triviality]") {
    REQUIRE(std::is_trivially_copy_constructible<tim::Result<int,int>>::value);
    REQUIRE(std::is_trivially_copy_assignable<tim::Result<int,int>>::value);
    REQUIRE(std::is_trivially_move_constructible<tim::Result<int,int>>::value);
    REQUIRE(std::is_trivially_move_assignable<tim::Result<int,int>>::value);
    REQUIRE(std::is_trivially_destructible<tim::Result<int,int>>::value);

    REQUIRE(std::is_trivially_copy_constructible<tim::Result<void,int>>::value);
    REQUIRE(std::is_trivially_move_constructible<tim::Result<void,int>>::value);
    REQUIRE(std::is_trivially_destructible<tim::Result<void,int>>::value);


    {
        struct T {
            T(const T&) = default;
            T(T&&) = default;
            T& operator=(const T&) = default;
            T& operator=(T&&) = default;
            ~T() = default;
        };
        REQUIRE(std::is_trivially_copy_constructible<tim::Result<T,int>>::value);
        REQUIRE(std::is_trivially_copy_assignable<tim::Result<T,int>>::value);
        REQUIRE(std::is_trivially_move_constructible<tim::Result<T,int>>::value);
        REQUIRE(std::is_trivially_move_assignable<tim::Result<T,int>>::value);
        REQUIRE(std::is_trivially_destructible<tim::Result<T,int>>::value);
    }

    {
        struct T {
            T(const T&){}
            T(T&&) {};
            T& operator=(const T&) {}
            T& operator=(T&&) {};
            ~T(){}
        };
        REQUIRE(!std::is_trivially_copy_constructible<tim::Result<T,int>>::value);
        REQUIRE(!std::is_trivially_copy_assignable<tim::Result<T,int>>::value);
        REQUIRE(!std::is_trivially_move_constructible<tim::Result<T,int>>::value);
        REQUIRE(!std::is_trivially_move_assignable<tim::Result<T,int>>::value);
        REQUIRE(!std::is_trivially_destructible<tim::Result<T,int>>::value);
    }

}

TEST_CASE("Deletion", "[bases.deletion]") {
    REQUIRE(std::is_copy_constructible<tim::Result<int,int>>::value);
    REQUIRE(std::is_copy_assignable<tim::Result<int,int>>::value);
    REQUIRE(std::is_move_constructible<tim::Result<int,int>>::value);
    REQUIRE(std::is_move_assignable<tim::Result<int,int>>::value);
    REQUIRE(std::is_destructible<tim::Result<int,int>>::value);

    {
        struct T {
            T()=default;
        };
        REQUIRE(std::is_default_constructible<tim::Result<T,int>>::value);
    }

    {
        struct T {
            T(int){}
        };
        REQUIRE(!std::is_default_constructible<tim::Result<T,int>>::value);
    }

    {
        struct T {
            T(const T&) = default;
            T(T&&) = default;
            T& operator=(const T&) = default;
            T& operator=(T&&) = default;
            ~T() = default;
        };
        REQUIRE(std::is_copy_constructible<tim::Result<T,int>>::value);
        REQUIRE(std::is_copy_assignable<tim::Result<T,int>>::value);
        REQUIRE(std::is_move_constructible<tim::Result<T,int>>::value);
        REQUIRE(std::is_move_assignable<tim::Result<T,int>>::value);
        REQUIRE(std::is_destructible<tim::Result<T,int>>::value);
    }

    {
        struct T {
            T(const T&)=delete;
            T(T&&)=delete;
            T& operator=(const T&)=delete;
            T& operator=(T&&)=delete;
        };
        REQUIRE(!std::is_copy_constructible<tim::Result<T,int>>::value);
        REQUIRE(!std::is_copy_assignable<tim::Result<T,int>>::value);
        REQUIRE(!std::is_move_constructible<tim::Result<T,int>>::value);
        REQUIRE(!std::is_move_assignable<tim::Result<T,int>>::value);
    }

    {
        struct T {
            T(const T&)=delete;
            T(T&&)=default;
            T& operator=(const T&)=delete;
            T& operator=(T&&)=default;
        };
        REQUIRE(!std::is_copy_constructible<tim::Result<T,int>>::value);
        REQUIRE(!std::is_copy_assignable<tim::Result<T,int>>::value);
        REQUIRE(std::is_move_constructible<tim::Result<T,int>>::value);
        REQUIRE(std::is_move_assignable<tim::Result<T,int>>::value);
    }

    {
        struct T {
            T(const T&)=default;
            T(T&&)=delete;
            T& operator=(const T&)=default;
            T& operator=(T&&)=delete;
        };
        REQUIRE(std::is_copy_constructible<tim::Result<T,int>>::value);
        REQUIRE(std::is_copy_assignable<tim::Result<T,int>>::value);
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

}


#endif

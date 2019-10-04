#include "catch.hpp"
#include "tim/result/Result.hpp"

struct no_throw {
	no_throw(std::string i) : i(i) {}
	std::string i;
};
struct canthrow_move {
	canthrow_move(std::string i) : i(i) {}
	canthrow_move(canthrow_move const &) = default;
	canthrow_move(canthrow_move &&other) noexcept(false) : i(other.i) {}
	canthrow_move &operator=(canthrow_move &&) = default;
	std::string i;
};

bool should_throw = false;
struct willthrow_move {
	willthrow_move(std::string i) : i(i) {}
	willthrow_move(willthrow_move const &) = default;
	willthrow_move(willthrow_move &&other) : i(other.i) {
		if (should_throw)
			throw 0;
	}
	willthrow_move &operator=(willthrow_move &&) = default;
	std::string i;
};
static_assert(std::is_swappable<no_throw>::value, "");

namespace test_adl {

enum class SpecialSwapTag {
	None, LHS, RHS, Other
};

struct HasSpecialSwap {
	SpecialSwapTag tag = SpecialSwapTag::None;
};

constexpr void swap(HasSpecialSwap& lhs, HasSpecialSwap& rhs) {
	lhs.tag = SpecialSwapTag::LHS;
	rhs.tag = SpecialSwapTag::RHS;
}

} /* namespace test_adl */

template <class T1, class T2> void swap_test() {
	std::string s1 = "abcdefghijklmnopqrstuvwxyz";
	std::string s2 = "zyxwvutsrqponmlkjihgfedcba";

	tim::Result<T1, T2> a{s1};
	tim::Result<T1, T2> b{s2};
	swap(a, b);
	REQUIRE(a->i == s2);
	REQUIRE(b->i == s1);

	a = s1;
	b = tim::Error<T2>(s2);
	swap(a, b);
	REQUIRE(a.error().i == s2);
	REQUIRE(b->i == s1);

	a = tim::Error<T2>(s1);
	b = s2;
	swap(a, b);
	REQUIRE(a->i == s2);
	REQUIRE(b.error().i == s1);

	a = tim::Error<T2>(s1);
	b = tim::Error<T2>(s2);
	swap(a, b);
	REQUIRE(a.error().i == s2);
	REQUIRE(b.error().i == s1);

	a = s1;
	b = s2;
	a.swap(b);
	REQUIRE(a->i == s2);
	REQUIRE(b->i == s1);

	a = s1;
	b = tim::Error<T2>(s2);
	a.swap(b);
	REQUIRE(a.error().i == s2);
	REQUIRE(b->i == s1);

	a = tim::Error<T2>(s1);
	b = s2;
	a.swap(b);
	REQUIRE(a->i == s2);
	REQUIRE(b.error().i == s1);

	a = tim::Error<T2>(s1);
	b = tim::Error<T2>(s2);
	a.swap(b);
	REQUIRE(a.error().i == s2);
	REQUIRE(b.error().i == s1);
}

TEST_CASE("swap") {

	swap_test<no_throw, no_throw>();
	swap_test<no_throw, canthrow_move>();
	swap_test<canthrow_move, no_throw>();

	std::string s1 = "abcdefghijklmnopqrstuvwxyz";
	std::string s2 = "zyxwvutsrqponmlkjihgfedcbaxxx";
	tim::Result<no_throw, willthrow_move> a{s1};
	tim::Result<no_throw, willthrow_move> b{tim::in_place_error, s2};
	should_throw = 1;


	#ifdef _MSC_VER
	//this seems to break catch on GCC and Clang
	REQUIRE_THROWS(swap(a, b));
	#endif

	REQUIRE(a->i == s1);
	REQUIRE(b.error().i == s2);

	{
		using test_adl::SpecialSwapTag;
		tim::Result<test_adl::HasSpecialSwap, int> a;
		tim::Result<test_adl::HasSpecialSwap, int> b;

		a.value().tag = SpecialSwapTag::None;
		b.value().tag = SpecialSwapTag::None;
		swap(a, b);
		REQUIRE(a.value().tag == SpecialSwapTag::LHS);
		REQUIRE(b.value().tag == SpecialSwapTag::RHS);

		a.value().tag = SpecialSwapTag::None;
		b.value().tag = SpecialSwapTag::None;
		tim::result::swap(a, b);
		REQUIRE(a.value().tag == SpecialSwapTag::LHS);
		REQUIRE(b.value().tag == SpecialSwapTag::RHS);
		
		a.value().tag = SpecialSwapTag::None;
		b.value().tag = SpecialSwapTag::None;
		tim::result::swap(b, a);
		REQUIRE(a.value().tag == SpecialSwapTag::RHS);
		REQUIRE(b.value().tag == SpecialSwapTag::LHS);
		
		a.value().tag = SpecialSwapTag::None;
		b.value().tag = SpecialSwapTag::None;
		a.swap(b);
		REQUIRE(a.value().tag == SpecialSwapTag::LHS);
		REQUIRE(b.value().tag == SpecialSwapTag::RHS);
		
		a.value().tag = SpecialSwapTag::None;
		b.value().tag = SpecialSwapTag::None;
		b.swap(a);
		REQUIRE(a.value().tag == SpecialSwapTag::RHS);
		REQUIRE(b.value().tag == SpecialSwapTag::LHS);

		a.emplace(test_adl::HasSpecialSwap{SpecialSwapTag::Other});
		b = tim::Error(-1);
		swap(a, b);
		REQUIRE(a == tim::Error(-1));
		REQUIRE(b.value().tag == SpecialSwapTag::Other);

		a.emplace(test_adl::HasSpecialSwap{SpecialSwapTag::Other});
		b = tim::Error(-1);
		swap(b, a);
		REQUIRE(a == tim::Error(-1));
		REQUIRE(b.value().tag == SpecialSwapTag::Other);

		a = tim::Error(-1);
		b.emplace(test_adl::HasSpecialSwap{SpecialSwapTag::Other});
		a.swap(b);
		REQUIRE(a.value().tag == SpecialSwapTag::Other);
		REQUIRE(b == tim::Error(-1));

		a = tim::Error(-1);
		b.emplace(test_adl::HasSpecialSwap{SpecialSwapTag::Other});
		b.swap(a);
		REQUIRE(a.value().tag == SpecialSwapTag::Other);
		REQUIRE(b == tim::Error(-1));

	}

	{
		using test_adl::SpecialSwapTag;
		tim::Result<int, test_adl::HasSpecialSwap> a(tim::in_place_error);
		tim::Result<int, test_adl::HasSpecialSwap> b(tim::in_place_error);

		a.error().tag = SpecialSwapTag::None;
		b.error().tag = SpecialSwapTag::None;
		swap(a, b);
		REQUIRE(a.error().tag == SpecialSwapTag::LHS);
		REQUIRE(b.error().tag == SpecialSwapTag::RHS);

		a.error().tag = SpecialSwapTag::None;
		b.error().tag = SpecialSwapTag::None;
		tim::result::swap(a, b);
		REQUIRE(a.error().tag == SpecialSwapTag::LHS);
		REQUIRE(b.error().tag == SpecialSwapTag::RHS);
		
		a.error().tag = SpecialSwapTag::None;
		b.error().tag = SpecialSwapTag::None;
		tim::result::swap(b, a);
		REQUIRE(a.error().tag == SpecialSwapTag::RHS);
		REQUIRE(b.error().tag == SpecialSwapTag::LHS);
		
		a.error().tag = SpecialSwapTag::None;
		b.error().tag = SpecialSwapTag::None;
		a.swap(b);
		REQUIRE(a.error().tag == SpecialSwapTag::LHS);
		REQUIRE(b.error().tag == SpecialSwapTag::RHS);
		
		a.error().tag = SpecialSwapTag::None;
		b.error().tag = SpecialSwapTag::None;
		b.swap(a);
		REQUIRE(a.error().tag == SpecialSwapTag::RHS);
		REQUIRE(b.error().tag == SpecialSwapTag::LHS);

		a = tim::Error(test_adl::HasSpecialSwap{SpecialSwapTag::Other});
		b = -1;
		swap(a, b);
		REQUIRE(a == -1);
		REQUIRE(b.error().tag == SpecialSwapTag::Other);

		a = tim::Error(test_adl::HasSpecialSwap{SpecialSwapTag::Other});
		b = -1;
		swap(b, a);
		REQUIRE(a == -1);
		REQUIRE(b.error().tag == SpecialSwapTag::Other);

		a = -1;
		b = tim::Error(test_adl::HasSpecialSwap{SpecialSwapTag::Other});
		a.swap(b);
		REQUIRE(a.error().tag == SpecialSwapTag::Other);
		REQUIRE(b == -1);

		a = -1;
		b = tim::Error(test_adl::HasSpecialSwap{SpecialSwapTag::Other});
		b.swap(a);
		REQUIRE(a.error().tag == SpecialSwapTag::Other);
		REQUIRE(b == -1);

	}

}

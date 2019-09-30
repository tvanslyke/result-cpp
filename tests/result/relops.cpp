#include "catch.hpp"
#include "tim/result/Result.hpp"
#include <string>

struct MyBool {
	bool value;
	constexpr explicit MyBool(bool v) : value(v) {}
	constexpr operator bool() const noexcept { return value; }
};

struct ComparesToMyBool {
	int value = 0;
};

inline constexpr MyBool operator==(const ComparesToMyBool& LHS, const ComparesToMyBool& RHS) noexcept {
	return MyBool(LHS.value == RHS.value);
}
inline constexpr MyBool operator!=(const ComparesToMyBool& LHS, const ComparesToMyBool& RHS) noexcept {
	return MyBool(LHS.value != RHS.value);
}
inline constexpr MyBool operator<(const ComparesToMyBool& LHS, const ComparesToMyBool& RHS) noexcept {
	return MyBool(LHS.value < RHS.value);
}
inline constexpr MyBool operator<=(const ComparesToMyBool& LHS, const ComparesToMyBool& RHS) noexcept {
	return MyBool(LHS.value <= RHS.value);
}
inline constexpr MyBool operator>(const ComparesToMyBool& LHS, const ComparesToMyBool& RHS) noexcept {
	return MyBool(LHS.value > RHS.value);
}
inline constexpr MyBool operator>=(const ComparesToMyBool& LHS, const ComparesToMyBool& RHS) noexcept {
	return MyBool(LHS.value >= RHS.value);
}

template <class L, class R>
struct is_equality_comparable {
	template <class T>
	struct Tag { };
	template <class T, class U, class = decltype(std::declval<T>() == std::declval<U>())>
	static constexpr auto test(Tag<T>, Tag<U>, int) -> std::true_type;
	template <class T, class U>
	static constexpr auto test(Tag<T>, Tag<U>, ...) -> std::false_type;

	static constexpr bool value = decltype(test(Tag<L>{}, Tag<R>{}, 0))::value;
};

template <class L, class R>
inline constexpr bool is_equality_comparable_v = is_equality_comparable<L, R>::value;

template <class L, class R>
struct is_inequality_comparable {
	template <class T>
	struct Tag { };
	template <class T, class U, class = decltype(std::declval<T>() != std::declval<U>())>
	static constexpr auto test(Tag<T>, Tag<U>, int) -> std::true_type;
	template <class T, class U>
	static constexpr auto test(Tag<T>, Tag<U>, ...) -> std::false_type;

	static constexpr bool value = decltype(test(Tag<L>{}, Tag<R>{}, 0))::value;
};

template <class L, class R>
inline constexpr bool is_inequality_comparable_v = is_inequality_comparable<L, R>::value;

template <class T1, class T2>
void test_equality_basic() {
	using tim::in_place;
	using tim::in_place_error;
	{
		using V = tim::Result<T1, T2>;
		constexpr V v1(in_place, T1{42});
		constexpr V v2(in_place, T1{42});
		static_assert(v1 == v2, "");
		static_assert(v2 == v1, "");
		static_assert(!(v1 != v2), "");
		static_assert(!(v2 != v1), "");
	}
	{
		using V = tim::Result<T1, T2>;
		constexpr V v1(in_place, T1{42});
		constexpr V v2(in_place, T1{43});
		static_assert(!(v1 == v2), "");
		static_assert(!(v2 == v1), "");
		static_assert(v1 != v2, "");
		static_assert(v2 != v1, "");
	}
	{
		using V = tim::Result<T1, T2>;
		constexpr V v1(in_place, T1{42});
		constexpr V v2(in_place_error, T2{42});
		static_assert(!(v1 == v2), "");
		static_assert(!(v2 == v1), "");
		static_assert(v1 != v2, "");
		static_assert(v2 != v1, "");
	}
	{
		using V = tim::Result<T1, T2>;
		constexpr V v1(in_place_error, T2{42});
		constexpr V v2(in_place_error, T2{42});
		static_assert(v1 == v2, "");
		static_assert(v2 == v1, "");
		static_assert(!(v1 != v2), "");
		static_assert(!(v2 != v1), "");
	}
}

struct NotBool {};

struct EqualityComparable {};

constexpr bool operator==(const EqualityComparable&, const EqualityComparable&) { return true; }

struct InequalityComparable {};
constexpr bool operator!=(const InequalityComparable&, const InequalityComparable&) { return false; }

struct EqualityComparableNonBool {};
constexpr NotBool operator==(const EqualityComparableNonBool&, const EqualityComparableNonBool&) { return NotBool{}; }

struct InequalityComparableNonBool {};
constexpr NotBool operator!=(const InequalityComparableNonBool&, const InequalityComparableNonBool&) { return NotBool{}; }

template <bool B, class L, class R>
static bool test_equality_comparable() {
	if constexpr(B) {
		CHECK(is_equality_comparable_v<L, R>);
		CHECK(is_equality_comparable_v<R, L>);
		return is_equality_comparable_v<L, R> && is_equality_comparable_v<R, L>;
	} else {
		CHECK(!is_equality_comparable_v<L, R>);
		CHECK(!is_equality_comparable_v<R, L>);
		return !is_equality_comparable_v<L, R> && !is_equality_comparable_v<R, L>;
	}
}

template <bool B, class L, class R>
static bool test_inequality_comparable() {
	if constexpr(B) {
		CHECK(is_inequality_comparable_v<L, R>);
		CHECK(is_inequality_comparable_v<R, L>);
		return is_inequality_comparable_v<L, R> && is_inequality_comparable_v<R, L>;
	} else {
		CHECK(!is_inequality_comparable_v<L, R>);
		CHECK(!is_inequality_comparable_v<R, L>);
		return !is_inequality_comparable_v<L, R> && !is_inequality_comparable_v<R, L>;
	}
}


template <bool B, class L, class R>
static bool test_relops(const L& lhs, const R& rhs) {
	if constexpr(B) {
		CHECK(lhs == rhs);
		CHECK(rhs == lhs);
		CHECK(!(lhs != rhs));
		CHECK(!(rhs != lhs));
		return (lhs == rhs)
			&& (rhs == lhs)
			&& (!(lhs != rhs))
			&& (!(rhs != lhs));
	} else {
		CHECK(lhs != rhs);
		CHECK(rhs != lhs);
		CHECK(!(lhs == rhs));
		CHECK(!(rhs == lhs));
		return (lhs != rhs)
			&& (rhs != lhs)
			&& (!(lhs == rhs))
			&& (!(rhs == lhs));
	}
}

TEST_CASE("Equality/Inequality", "[relops]") {
	test_equality_basic<int, long>();
	test_equality_basic<ComparesToMyBool, int>();
	test_equality_basic<int, ComparesToMyBool>();
	test_equality_basic<ComparesToMyBool, ComparesToMyBool>();
	struct T{};
	struct U{};
	using tim::Result;
	using tim::Error;

	REQUIRE(test_equality_comparable<false, Result<T, int>, Result<U, int>>());
	REQUIRE(test_equality_comparable<false, Result<T, int>, Result<T, int>>());
	REQUIRE(test_equality_comparable<true, Result<std::string, int>, Result<const char*, int>>);

	REQUIRE(test_inequality_comparable<false, Result<T, int>, Result<U, int>>());
	REQUIRE(test_inequality_comparable<false, Result<T, int>, Result<T, int>>());
	REQUIRE(test_inequality_comparable<true, Result<std::string, int>, Result<const char*, int>>());

	REQUIRE(test_equality_comparable<true, Result<void, const char*>, Result<void, std::string>>());
	REQUIRE(test_inequality_comparable<true, Result<void, const char*>, Result<void, std::string>>());

	REQUIRE(test_equality_comparable<false, Result<void, T>, Result<void, std::string>>());
	REQUIRE(test_inequality_comparable<false, Result<void, T>, Result<void, std::string>>());

	REQUIRE(test_equality_comparable<true, Result<void, int>, Error<unsigned>>());
	REQUIRE(test_inequality_comparable<true, Result<void, int>, Error<unsigned>>());
	
	REQUIRE(test_equality_comparable<true, Result<int, int>, Error<unsigned>>());
	REQUIRE(test_inequality_comparable<true, Result<int, int>, Error<unsigned>>());
	
	REQUIRE(test_equality_comparable<false, Result<int, T>, Error<unsigned>>());
	REQUIRE(test_inequality_comparable<false, Result<int, T>, Error<unsigned>>());

	REQUIRE(test_equality_comparable<true, Result<int, EqualityComparable>, Result<int, EqualityComparable>>());
	REQUIRE(test_equality_comparable<true, Result<void, EqualityComparable>, Result<void, EqualityComparable>>());
	REQUIRE(test_equality_comparable<true, Result<EqualityComparable, int>, Result<EqualityComparable, int>>());
	REQUIRE(test_equality_comparable<true, Result<int, EqualityComparable>, Error<EqualityComparable>>());
	REQUIRE(test_equality_comparable<true, Result<void, EqualityComparable>, Error<EqualityComparable>>());
	REQUIRE(test_equality_comparable<true, Result<EqualityComparable, int>, EqualityComparable>());

	REQUIRE(test_equality_comparable<false, Result<int, InequalityComparable>, Result<int, InequalityComparable>>());
	REQUIRE(test_equality_comparable<false, Result<void, InequalityComparable>, Result<void, InequalityComparable>>());
	REQUIRE(test_equality_comparable<false, Result<InequalityComparable, int>, Result<InequalityComparable, int>>());
	REQUIRE(test_equality_comparable<false, Result<int, InequalityComparable>, Error<InequalityComparable>>());
	REQUIRE(test_equality_comparable<false, Result<void, InequalityComparable>, Error<InequalityComparable>>());
	REQUIRE(test_equality_comparable<false, Result<InequalityComparable, int>, InequalityComparable>());

	REQUIRE(test_equality_comparable<false, Result<int, EqualityComparableNonBool>, Result<int, EqualityComparableNonBool>>());
	REQUIRE(test_equality_comparable<false, Result<void, EqualityComparableNonBool>, Result<void, EqualityComparableNonBool>>());
	REQUIRE(test_equality_comparable<false, Result<EqualityComparableNonBool, int>, Result<EqualityComparableNonBool, int>>());
	REQUIRE(test_equality_comparable<false, Result<int, EqualityComparableNonBool>, Error<EqualityComparableNonBool>>());
	REQUIRE(test_equality_comparable<false, Result<void, EqualityComparableNonBool>, Error<EqualityComparableNonBool>>());
	REQUIRE(test_equality_comparable<false, Result<EqualityComparableNonBool, int>, EqualityComparableNonBool>());


	REQUIRE(test_inequality_comparable<true, Result<int, InequalityComparable>, Result<int, InequalityComparable>>());
	REQUIRE(test_inequality_comparable<true, Result<void, InequalityComparable>, Result<void, InequalityComparable>>());
	REQUIRE(test_inequality_comparable<true, Result<InequalityComparable, int>, Result<InequalityComparable, int>>());
	REQUIRE(test_inequality_comparable<true, Result<int, InequalityComparable>, Error<InequalityComparable>>());
	REQUIRE(test_inequality_comparable<true, Result<void, InequalityComparable>, Error<InequalityComparable>>());
	REQUIRE(test_inequality_comparable<true, Result<InequalityComparable, int>, InequalityComparable>());

	REQUIRE(test_inequality_comparable<false, Result<int, EqualityComparable>, Result<int, EqualityComparable>>());
	REQUIRE(test_inequality_comparable<false, Result<void, EqualityComparable>, Result<void, EqualityComparable>>());
	REQUIRE(test_inequality_comparable<false, Result<EqualityComparable, int>, Result<EqualityComparable, int>>());
	REQUIRE(test_inequality_comparable<false, Result<int, EqualityComparable>, Error<EqualityComparable>>());
	REQUIRE(test_inequality_comparable<false, Result<void, EqualityComparable>, Error<EqualityComparable>>());
	REQUIRE(test_inequality_comparable<false, Result<EqualityComparable, int>, EqualityComparable>());

	REQUIRE(test_inequality_comparable<false, Result<int, InequalityComparableNonBool>, Result<int, InequalityComparableNonBool>>());
	REQUIRE(test_inequality_comparable<false, Result<void, InequalityComparableNonBool>, Result<void, InequalityComparableNonBool>>());
	REQUIRE(test_inequality_comparable<false, Result<InequalityComparableNonBool, int>, Result<InequalityComparableNonBool, int>>());
	REQUIRE(test_inequality_comparable<false, Result<int, InequalityComparableNonBool>, Error<InequalityComparableNonBool>>());
	REQUIRE(test_inequality_comparable<false, Result<void, InequalityComparableNonBool>, Error<InequalityComparableNonBool>>());
	REQUIRE(test_inequality_comparable<false, Result<InequalityComparableNonBool, int>, InequalityComparableNonBool>());


	REQUIRE(test_relops<true>(Result<void, int>(Error(0)), Error(0)));
	REQUIRE(test_relops<false>(Result<void, int>(Error(0)), Error(1)));
	REQUIRE(test_relops<false>(Result<int, int>(1), Error(1)));
	REQUIRE(test_relops<true>(Result<int, int>(1), 1));
	REQUIRE(test_relops<false>(Result<int, int>(1), 2));
	REQUIRE(test_relops<false>(Result<std::string, int>("test"), Error(0)));
	REQUIRE(test_relops<false>(Result<const char*, int>("test"), Error(0)));
	REQUIRE(test_relops<true>(Result<const char*, int>(tim::in_place_error, 0), Error(0)));
	REQUIRE(test_relops<false>(Result<const char*, int>(tim::in_place_error, 1), Error(0)));
	REQUIRE(test_relops<true>(Result<int, std::string>(tim::in_place_error, "test"), Error("test")));
	REQUIRE(test_relops<true>(Result<int, std::string>(tim::in_place_error, "test"), Error("test")));
	REQUIRE(test_relops<false>(Result<int, std::string>(tim::in_place_error, "test"), 0));

}

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

template <bool B, class L, class R>
static void test_relops(const L& lhs, const R& rhs) {
	if constexpr(B) {
		REQUIRE(lhs == rhs);
		REQUIRE(rhs == lhs);
		REQUIRE(!(lhs != rhs));
		REQUIRE(!(rhs != lhs));
	} else {
		REQUIRE(lhs != rhs);
		REQUIRE(rhs != lhs);
		REQUIRE(!(lhs == rhs));
		REQUIRE(!(rhs == lhs));
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

	REQUIRE(!is_equality_comparable_v<Result<T, int>, Result<U, int>>);
	REQUIRE(!is_equality_comparable_v<Result<T, int>, Result<T, int>>);
	REQUIRE(is_equality_comparable_v<Result<std::string, int>, Result<const char*, int>>);
	REQUIRE(!is_inequality_comparable_v<Result<T, int>, Result<U, int>>);
	REQUIRE(!is_inequality_comparable_v<Result<T, int>, Result<T, int>>);
	REQUIRE(is_inequality_comparable_v<Result<std::string, int>, Result<const char*, int>>);

	REQUIRE(tim::traits::detail::is_contextually_convertible_to_bool_v<decltype(std::declval<std::string>() == std::declval<const char*>())>);
	REQUIRE(is_equality_comparable_v<Result<void, const char*>, Result<void, std::string>>);
	REQUIRE(is_inequality_comparable_v<Result<void, const char*>, Result<void, std::string>>);
	REQUIRE(!is_equality_comparable_v<Result<void, T>, Result<void, std::string>>);
	REQUIRE(!is_inequality_comparable_v<Result<void, T>, Result<void, std::string>>);

	REQUIRE(is_equality_comparable_v<Result<void, int>, Error<unsigned>>);
	REQUIRE(is_inequality_comparable_v<Result<void, int>, Error<unsigned>>);
	
	REQUIRE(is_equality_comparable_v<Result<int, int>, Error<unsigned>>);
	REQUIRE(is_inequality_comparable_v<Result<int, int>, Error<unsigned>>);
	
	REQUIRE(!is_equality_comparable_v<Result<int, T>, Error<unsigned>>);
	REQUIRE(!is_inequality_comparable_v<Result<int, T>, Error<unsigned>>);

	test_relops<true>(tim::Result<void, int>(Error(0)), Error(0));
	test_relops<false>(tim::Result<void, int>(Error(0)), Error(1));
	test_relops<false>(tim::Result<int, int>(1), Error(1));
	test_relops<true>(tim::Result<int, int>(1), 1);
	test_relops<false>(tim::Result<int, int>(1), 2);
	test_relops<false>(tim::Result<std::string, int>("test"), Error(0));
	test_relops<false>(tim::Result<const char*, int>("test"), Error(0));
	test_relops<true>(tim::Result<const char*, int>(tim::in_place_error, 0), Error(0));
	test_relops<true>(tim::Result<const char*, int>(tim::in_place_error, 1), Error(0));
	test_relops<true>(tim::Result<int, std::string>(tim::in_place_error, "test"), Error("test"));
	test_relops<true>(tim::Result<int, std::string>(tim::in_place_error, "test"), Error("test"));
	test_relops<false>(tim::Result<int, std::string>(tim::in_place_error, "test"), 0);

}

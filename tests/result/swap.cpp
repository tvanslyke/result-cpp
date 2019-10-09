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

namespace swap_test_namespace {

namespace detail {

template <
	class T,
	std::enable_if_t<
		std::is_same_v<decltype(std::declval<T&>().swap(std::declval<T&>())), void>
		&& std::is_same_v<decltype(&T::template swap<T>), void (T::*)(T&)>
		&& std::is_invocable_r_v<void, decltype(&T::template swap<T>), T&, T&>,
		bool
	> = false
>
static constexpr std::true_type is_member_template_swappable_helper(int, int) { return std::true_type{}; }

template <class T>
static constexpr std::false_type is_member_template_swappable_helper(int, ...) { return std::false_type{}; }

template <
	class T,
	std::enable_if_t<
		std::is_same_v<decltype(std::declval<T&>().swap(std::declval<T&>())), void>
		&& std::is_same_v<decltype(&T::swap), void (T::*)(T&)>
		&& std::is_invocable_r_v<void, decltype(&T::swap), T&, T&>,
		bool
	> = false
>
static constexpr std::true_type is_member_non_template_swappable_helper(int, int) { return std::true_type{}; }

template <class T>
static constexpr std::false_type is_member_non_template_swappable_helper(int, ...) { return std::false_type{}; }

template <class T>
static constexpr auto is_member_swappable_helper(int, int)
	-> std::bool_constant<
		is_member_template_swappable_helper<T>(0, 0)
		|| is_member_non_template_swappable_helper<T>(0, 0)
	>
{
	return std::bool_constant<
		is_member_template_swappable_helper<T>(0, 0)
		|| is_member_non_template_swappable_helper<T>(0, 0)
	>{};
}

template <class T, class = decltype(swap(std::declval<T&>(), std::declval<T&>()))>
static constexpr std::true_type  is_non_member_swappable_helper(int, int) { return std::true_type{}; }

template <class T>
static constexpr std::false_type is_non_member_swappable_helper(int, ...) { return std::false_type{}; }

template <class T, class = decltype(std::swap(std::declval<T&>(), std::declval<T&>()))>
static constexpr std::true_type  is_std_swappable_helper(int, int) { return std::true_type{}; }

template <class T>
static constexpr std::false_type is_std_swappable_helper(int, ...) { return std::false_type{}; }

} /* namespace detail */ 

template <class T>
struct is_member_swappable:
	decltype(detail::is_member_swappable_helper<T>(0, 0))
{

};

template <class T>
inline constexpr bool is_member_swappable_v = is_member_swappable<T>::value;

template <class T>
struct is_non_member_swappable:
	decltype(detail::is_non_member_swappable_helper<T>(0, 0))
{

};

template <class T>
inline constexpr bool is_non_member_swappable_v = is_non_member_swappable<T>::value;

template <class T>
struct is_std_swappable:
	decltype(detail::is_std_swappable_helper<T>(0, 0))
{

};

template <class T>
inline constexpr bool is_std_swappable_v = is_std_swappable<T>::value;

struct NotSwappable {};
void swap(NotSwappable &, NotSwappable &) = delete;

struct NotCopyable {
	NotCopyable() = default;
	NotCopyable(const NotCopyable &) = delete;
	NotCopyable &operator=(const NotCopyable &) = delete;
};

struct NotCopyableWithSwap {
	NotCopyableWithSwap() = default;
	NotCopyableWithSwap(const NotCopyableWithSwap &) = delete;
	NotCopyableWithSwap &operator=(const NotCopyableWithSwap &) = delete;
};
void swap(NotCopyableWithSwap &, NotCopyableWithSwap) {}

struct NotMoveAssignable {
	NotMoveAssignable() = default;
	NotMoveAssignable(NotMoveAssignable &&) = default;
	NotMoveAssignable &operator=(NotMoveAssignable &&) = delete;
};

struct NotMoveAssignableWithSwap {
	NotMoveAssignableWithSwap() = default;
	NotMoveAssignableWithSwap(NotMoveAssignableWithSwap &&) = default;
	NotMoveAssignableWithSwap &operator=(NotMoveAssignableWithSwap &&) = delete;
};
void swap(NotMoveAssignableWithSwap &, NotMoveAssignableWithSwap &) noexcept {}

template <bool Throws> void do_throw() {}

template <> void do_throw<true>() {
	throw 42;
}

template <bool NT_Copy, bool NT_Move, bool NT_CopyAssign, bool NT_MoveAssign,
					bool NT_Swap, bool EnableSwap = true>
struct NothrowTypeImp {
	static int move_called;
	static int move_assign_called;
	static int swap_called;
	static void reset() { move_called = move_assign_called = swap_called = 0; }
	NothrowTypeImp() = default;
	explicit NothrowTypeImp(int v) : value(v) {}
	NothrowTypeImp(const NothrowTypeImp &o) noexcept(NT_Copy) : value(o.value) {
		REQUIRE(false);
	} // never called by test
	NothrowTypeImp(NothrowTypeImp &&o) noexcept(NT_Move) : value(o.value) {
		++move_called;
		do_throw<!NT_Move>();
		o.value = -1;
	}
	NothrowTypeImp &operator=(const NothrowTypeImp &) noexcept(NT_CopyAssign) {
		REQUIRE(false);
		return *this;
	} // never called by the tests
	NothrowTypeImp &operator=(NothrowTypeImp &&o) noexcept(NT_MoveAssign) {
		++move_assign_called;
		do_throw<!NT_MoveAssign>();
		value = o.value;
		o.value = -1;
		return *this;
	}
	int value;
};
template <bool NT_Copy, bool NT_Move, bool NT_CopyAssign, bool NT_MoveAssign,
					bool NT_Swap, bool EnableSwap>
int NothrowTypeImp<NT_Copy, NT_Move, NT_CopyAssign, NT_MoveAssign, NT_Swap,
									 EnableSwap>::move_called = 0;
template <bool NT_Copy, bool NT_Move, bool NT_CopyAssign, bool NT_MoveAssign,
					bool NT_Swap, bool EnableSwap>
int NothrowTypeImp<NT_Copy, NT_Move, NT_CopyAssign, NT_MoveAssign, NT_Swap,
									 EnableSwap>::move_assign_called = 0;
template <bool NT_Copy, bool NT_Move, bool NT_CopyAssign, bool NT_MoveAssign,
					bool NT_Swap, bool EnableSwap>
int NothrowTypeImp<NT_Copy, NT_Move, NT_CopyAssign, NT_MoveAssign, NT_Swap,
									 EnableSwap>::swap_called = 0;

template <bool NT_Copy, bool NT_Move, bool NT_CopyAssign, bool NT_MoveAssign,
					bool NT_Swap>
void swap(NothrowTypeImp<NT_Copy, NT_Move, NT_CopyAssign, NT_MoveAssign,
												 NT_Swap, true> &lhs,
					NothrowTypeImp<NT_Copy, NT_Move, NT_CopyAssign, NT_MoveAssign,
												 NT_Swap, true> &rhs) noexcept(NT_Swap) {
	lhs.swap_called++;
	do_throw<!NT_Swap>();
	int tmp = lhs.value;
	lhs.value = rhs.value;
	rhs.value = tmp;
}

// throwing copy, nothrow move ctor/assign, no swap provided
using NothrowMoveable = NothrowTypeImp<false, true, false, true, false, false>;
// throwing copy and move assign, nothrow move ctor, no swap provided
using NothrowMoveCtor = NothrowTypeImp<false, true, false, false, false, false>;
// nothrow move ctor, throwing move assignment, swap provided
using NothrowMoveCtorWithThrowingSwap =
		NothrowTypeImp<false, true, false, false, false, true>;
// throwing move ctor, nothrow move assignment, no swap provided
using ThrowingMoveCtor =
		NothrowTypeImp<false, false, false, true, false, false>;
// throwing special members, nothrowing swap
using ThrowingTypeWithNothrowSwap =
		NothrowTypeImp<false, false, false, false, true, true>;
using NothrowTypeWithThrowingSwap =
		NothrowTypeImp<true, true, true, true, false, true>;
// throwing move assign with nothrow move and nothrow swap
using ThrowingMoveAssignNothrowMoveCtorWithSwap =
		NothrowTypeImp<false, true, false, false, true, true>;
// throwing move assign with nothrow move but no swap.
using ThrowingMoveAssignNothrowMoveCtor =
		NothrowTypeImp<false, true, false, false, false, false>;

struct NonThrowingNonNoexceptType {
	static int move_called;
	static void reset() { move_called = 0; }
	NonThrowingNonNoexceptType() = default;
	NonThrowingNonNoexceptType(int v) : value(v) {}
	NonThrowingNonNoexceptType(NonThrowingNonNoexceptType &&o) noexcept(false)
			: value(o.value) {
		++move_called;
		o.value = -1;
	}
	NonThrowingNonNoexceptType &
	operator=(NonThrowingNonNoexceptType &&) noexcept(false) {
		REQUIRE(false); // never called by the tests.
		return *this;
	}
	int value;
};
int NonThrowingNonNoexceptType::move_called = 0;

struct ThrowsOnSecondMove {
	int value;
	int move_count;
	ThrowsOnSecondMove(int v) : value(v), move_count(0) {}
	ThrowsOnSecondMove(ThrowsOnSecondMove &&o) noexcept(false)
			: value(o.value), move_count(o.move_count + 1) {
		if (move_count == 2)
			do_throw<true>();
		o.value = -1;
	}
	ThrowsOnSecondMove &operator=(ThrowsOnSecondMove &&) {
		REQUIRE(false); // not called by test
		return *this;
	}
};


TEST_CASE("Swap same value") {
	{
		using T = ThrowingTypeWithNothrowSwap;
		using V = tim::Result<T, int>;
		T::reset();
		V v1(tim::in_place, 42);
		V v2(tim::in_place, 100);
		v1.swap(v2);
		REQUIRE(T::swap_called == 1);
		REQUIRE(v1.value().value == 100);
		REQUIRE(v2.value().value == 42);
		swap(v1, v2);
		REQUIRE(T::swap_called == 2);
		REQUIRE(v1.value().value == 42);
		REQUIRE(v2.value().value == 100);
	}
	{
		using T = NothrowMoveable;
		using V = tim::Result<T, int>;
		T::reset();
		V v1(tim::in_place, 42);
		V v2(tim::in_place, 100);
		v1.swap(v2);
		REQUIRE(T::swap_called == 0);
		REQUIRE(T::move_called == 1);
		REQUIRE(T::move_assign_called == 2);
		REQUIRE(v1.value().value == 100);
		REQUIRE(v2.value().value == 42);
		T::reset();
		swap(v1, v2);
		REQUIRE(T::swap_called == 0);
		REQUIRE(T::move_called == 1);
		REQUIRE(T::move_assign_called == 2);
		REQUIRE(v1.value().value == 42);
		REQUIRE(v2.value().value == 100);
	}
	{
		using T = NothrowTypeWithThrowingSwap;
		using V = tim::Result<T, int>;
		T::reset();
		V v1(tim::in_place, 42);
		V v2(tim::in_place, 100);
		try {
			v1.swap(v2);
			REQUIRE(false);
		} catch (int) {
		}
		REQUIRE(T::swap_called == 1);
		REQUIRE(T::move_called == 0);
		REQUIRE(T::move_assign_called == 0);
		REQUIRE(v1.value().value == 42);
		REQUIRE(v2.value().value == 100);
	}
	{
		using T = ThrowingMoveCtor;
		using V = tim::Result<T, int>;
		T::reset();
		V v1(tim::in_place, 42);
		V v2(tim::in_place, 100);
		try {
			v1.swap(v2);
			REQUIRE(false);
		} catch (int) {
		}
		REQUIRE(T::move_called == 1); // call threw
		REQUIRE(T::move_assign_called == 0);
		REQUIRE(v1.value().value ==
					 42); // throw happened before v1 was moved from
		REQUIRE(v2.value().value == 100);
	}
	{
		using T = ThrowingMoveAssignNothrowMoveCtor;
		using V = tim::Result<T, int>;
		T::reset();
		V v1(tim::in_place, 42);
		V v2(tim::in_place, 100);
		try {
			v1.swap(v2);
			REQUIRE(false);
		} catch (int) {
		}
		REQUIRE(T::move_called == 1);
		REQUIRE(T::move_assign_called == 1);	// call threw and didn't complete
		REQUIRE(v1.value().value == -1); // v1 was moved from
		REQUIRE(v2.value().value == 100);
	}
}

void test_swap_different_alternatives() {
	{
		using T1 = NothrowMoveCtorWithThrowingSwap;
		using T2 = int;
		using V = tim::Result<T1, T2>;
		if constexpr(is_member_swappable_v<V>) {
			REQUIRE(std::is_swappable_v<V>);
			REQUIRE(is_std_swappable_v<V>);
			REQUIRE(is_non_member_swappable_v<V>);
			REQUIRE(is_member_swappable_v<V>);
			T1::reset();
			V v1(tim::in_place, 42);
			V v2(tim::in_place_error, 100);
			v1.swap(v2);
			REQUIRE(T1::swap_called == 0);
			// The libc++ implementation double copies the argument, and not
			// the variant swap is called on.
			REQUIRE(T1::move_called == 1);
			REQUIRE(T1::move_called <= 2);
			REQUIRE(T1::move_assign_called == 0);
			REQUIRE(v1.error() == 100);
			REQUIRE(v2.value().value == 42);
			T1::reset();
			swap(v1, v2);
			REQUIRE(T1::swap_called == 0);
			REQUIRE(T1::move_called == 2);
			REQUIRE(T1::move_called <= 2);
			REQUIRE(T1::move_assign_called == 0);
			REQUIRE(v1.value().value == 42);
			REQUIRE(v2.error() == 100);
		} else {
			REQUIRE(std::is_swappable_v<V>);
			REQUIRE(is_std_swappable_v<V>);
			REQUIRE(!is_non_member_swappable_v<V>);
			REQUIRE(!is_member_swappable_v<V>);
		}
	}
	{

		using T1 = ThrowingTypeWithNothrowSwap;
		using T2 = NothrowMoveable;
		using V = tim::Result<T1, T2>;
		if constexpr(is_member_swappable_v<V>) {
			REQUIRE(std::is_swappable_v<V>);
			REQUIRE(is_std_swappable_v<V>);
			REQUIRE(is_non_member_swappable_v<V>);
			REQUIRE(is_member_swappable_v<V>);
			T1::reset();
			T2::reset();
			V v1(tim::in_place, 42);
			V v2(tim::in_place_error, 100);
			try {
				v1.swap(v2);
				REQUIRE(false);
			} catch (int) {
			}
			REQUIRE(T1::swap_called == 0);
			REQUIRE(T1::move_called == 1);
			REQUIRE(T1::move_assign_called == 0);
			REQUIRE(T2::swap_called == 0);
			REQUIRE(T2::move_called == 2);
			REQUIRE(T2::move_assign_called == 0);
			REQUIRE(v1.value().value == 42);
			REQUIRE(v2.error().value == 100);
			// swap again, but call v2's swap.
			T1::reset();
			T2::reset();
			try {
				v2.swap(v1);
				REQUIRE(false);
			} catch (int) {
			}
			REQUIRE(T1::swap_called == 0);
			REQUIRE(T1::move_called == 1);
			REQUIRE(T1::move_assign_called == 0);
			REQUIRE(T2::swap_called == 0);
			REQUIRE(T2::move_called == 2);
			REQUIRE(T2::move_assign_called == 0);
			REQUIRE(v1.value().value == 42);
			REQUIRE(v2.error().value == 100);
		} else {
			REQUIRE(std::is_swappable_v<V>);
			REQUIRE(is_std_swappable_v<V>);
			REQUIRE(!is_non_member_swappable_v<V>);
			REQUIRE(!is_member_swappable_v<V>);
		}
	}
}

template <class Var>
constexpr auto has_swap_member_imp(int)
		-> decltype(std::declval<Var &>().swap(std::declval<Var &>()), true) {
	return true;
}

template <class Var> constexpr auto has_swap_member_imp(long) -> bool {
	return false;
}

template <class Var> constexpr bool has_swap_member() {
	return has_swap_member_imp<Var>(0);
}

namespace types {

enum class Kind {
	Deleted,
	Nothrow,
	Throwing
};

template <class T, bool Nothrow>
struct Swappable: T {
	Swappable() = default;
	Swappable(Swappable&&) noexcept(std::is_nothrow_move_constructible_v<T>) = default;
	Swappable(const Swappable&) noexcept(std::is_nothrow_move_constructible_v<T>) = default;
	Swappable& operator=(const Swappable&) noexcept(std::is_nothrow_copy_assignable_v<T>) = default;
	Swappable& operator=(Swappable&&) noexcept(std::is_nothrow_move_assignable_v<T>) = default;
};

template <class T, bool NT>
constexpr void swap(Swappable<T, NT>&, Swappable<T, NT>&) noexcept(NT) {}

struct NotMoveConstructible {
	NotMoveConstructible() {};
	NotMoveConstructible(NotMoveConstructible&&) = delete;
	NotMoveConstructible(const NotMoveConstructible&) {};
	NotMoveConstructible& operator=(NotMoveConstructible&&) { return *this; };
	NotMoveConstructible& operator=(const NotMoveConstructible&) { return *this; };
};

struct NotMoveAssignable {
	NotMoveAssignable() {};
	NotMoveAssignable(NotMoveAssignable&&) {};
	NotMoveAssignable(const NotMoveAssignable&)  {};
	NotMoveAssignable& operator=(NotMoveAssignable&&) = delete;
	NotMoveAssignable& operator=(const NotMoveAssignable&) { return *this; };
};
 
struct NotCopyConstructible {
	NotCopyConstructible() {};
	NotCopyConstructible(NotCopyConstructible&&) = delete;
	NotCopyConstructible(const NotCopyConstructible&) {};
	NotCopyConstructible& operator=(NotCopyConstructible&&) { return *this; };
	NotCopyConstructible& operator=(const NotCopyConstructible&) { return *this; };
};

struct NotCopyAssignable {
	NotCopyAssignable() {};
	NotCopyAssignable(NotCopyAssignable&&) {};
	NotCopyAssignable(const NotCopyAssignable&) {};
	NotCopyAssignable& operator=(NotCopyAssignable&&) { return *this; }
	NotCopyAssignable& operator=(const NotCopyAssignable&) = delete;
};

struct Dummy {};

template <
	Kind MoveCtorKind,
	Kind CopyCtorKind,
	Kind MoveAsgnKind,
	Kind CopyAsgnKind,
	Kind SwapKind
>
struct Type;
template <
	Kind MoveCtorKind,
	Kind CopyCtorKind,
	Kind MoveAsgnKind,
	Kind CopyAsgnKind,
	Kind SwapKind,
	std::enable_if_t<(SwapKind == Kind::Deleted), bool> = false
>
constexpr void swap(
	Type<MoveCtorKind, CopyCtorKind, MoveAsgnKind, CopyAsgnKind, Kind::Deleted>&,
	Type<MoveCtorKind, CopyCtorKind, MoveAsgnKind, CopyAsgnKind, Kind::Deleted>&
) = delete;

template <
	Kind MoveCtorKind,
	Kind CopyCtorKind,
	Kind MoveAsgnKind,
	Kind CopyAsgnKind,
	Kind SwapKind,
	std::enable_if_t<(SwapKind == Kind::Throwing), bool> = false
>
constexpr void swap(
	Type<MoveCtorKind, CopyCtorKind, MoveAsgnKind, CopyAsgnKind, Kind::Deleted>&,
	Type<MoveCtorKind, CopyCtorKind, MoveAsgnKind, CopyAsgnKind, Kind::Deleted>&
) noexcept(false) {}

template <
	Kind MoveCtorKind,
	Kind CopyCtorKind,
	Kind MoveAsgnKind,
	Kind CopyAsgnKind,
	Kind SwapKind,
	std::enable_if_t<(SwapKind == Kind::Nothrow), bool> = false
>
constexpr void swap(
	Type<MoveCtorKind, CopyCtorKind, MoveAsgnKind, CopyAsgnKind, Kind::Deleted>&,
	Type<MoveCtorKind, CopyCtorKind, MoveAsgnKind, CopyAsgnKind, Kind::Deleted>&
) noexcept(true) {}

template <class T, class E>
constexpr bool should_be_std_swappable {
	return ;
}

template <
	Kind MoveCtorKind,
	Kind CopyCtorKind,
	Kind MoveAsgnKind,
	Kind CopyAsgnKind,
	Kind SwapKind
>
struct TypeImpl:
	std::conditional_t<(MoveCtorKind != Kind::Deleted), Dummy, NotMoveConstructible>,
	std::conditional_t<(CopyCtorKind != Kind::Deleted), Dummy, NotCopyConstructible>,
	std::conditional_t<(MoveAsgnKind != Kind::Deleted), Dummy, NotMoveAssignable>,
	std::conditional_t<(CopyAsgnKind != Kind::Deleted), Dummy, NotCopyAssignable>
{
	TypeImpl() {};
	TypeImpl(TypeImpl&&) noexcept(MoveCtorKind == Kind::Nothrow) = default;
	TypeImpl(const TypeImpl&) noexcept(MoveCtorKind == Kind::Nothrow) = default;
	TypeImpl& operator=(TypeImpl&&) noexcept(MoveAsgnKind == Kind::Nothrow) = default;
	TypeImpl& operator=(const TypeImpl&) noexcept(CopyAsgnKind == Kind::Nothrow) = default;
};

template <
	Kind MoveCtorKind,
	Kind CopyCtorKind,
	Kind MoveAsgnKind,
	Kind CopyAsgnKind,
	Kind SwapKind
>
struct get_type_impl {
	using type = TypeImpl<
		MoveCtorKind,
		CopyCtorKind,
		MoveAsgnKind,
		CopyAsgnKind,
		SwapKind
	>;

	template <
		Kind K,
		template <class> class IsEnabled,
		template <class> class IsNothrow
	>
	using check_trait_type = std::conditional_t<
		(K == Kind::Deleted),
		std::negation<IsEnabled<type>>,
		std::conditional_t<
			(K == Kind::Throwing),
			std::conjunction<
				IsEnabled<type>,
				std::negation<IsNothrow<type>>
			>,
			std::conjunction<
				IsEnabled<type>,
				IsNothrow<type>
			>
		>
	>;
		
	static_assert(
		check_type_trait<
			MoveCtorKind,
			std::is_move_constructible,
			std::is_nothrow_move_construtible
		>
	);
	static_assert(
		check_type_trait<
			CopyCtorKind,
			std::is_copy_constructible,
			std::is_nothrow_copy_construtible
		>
	);
	static_assert(
		check_type_trait<
			MoveAsgnKind,
			std::is_move_assignable,
			std::is_nothrow_move_construtible
		>
	);
	static_assert(
		check_type_trait<
			CopyAsgnKind,
			std::is_copy_assignable,
			std::is_nothrow_copy_construtible
		>
	);
	static_assert(
		check_type_trait<
			SwapKind,
			std::swappable,
			std::is_nothrow_swappable
		>
	);
};

template <
	Kind MoveCtorKind,
	Kind CopyCtorKind,
	Kind MoveAsgnKind,
	Kind CopyAsgnKind,
	Kind SwapKind
>
using Type = typename get_type_impl<
	MoveCtorKind,
	CopyCtorKind,
	MoveAsgnKind,
	CopyAsgnKind,
	SwapKind
>::type;


template <std::size_t Index>
struct kind_encoding {
	static constexpr Kind to_kind(std::size_t index) {
		switch(static_cast<Kind>(index)) {
		default: REQUIRE(false);
		case 0: return Kind::Deleted;
		case 1: return Kind::Nothrow;
		case 2: return Kind::Throwing;
		}
	}
	static constexpr Kind move_ctor_kind() {
		return to_kind(Index % 3);
	}
	static constexpr Kind copy_ctor_kind() {
		return to_kind((Index / 3) % 3);
	}
	static constexpr Kind move_asgn_kind() {
		return to_kind((Index / 9) % 3);
	}
	static constexpr Kind copy_asgn_kind() {
		return to_kind((Index / 27) % 3);
	}
	static constexpr Kind swap_kind() {
		return to_kind((Index / 81) % 3);
	}
	using type = Type<
		move_ctor_kind(),
		copy_ctor_kind(),
		move_asgn_kind(),
		copy_asgn_kind(),
		swap_kind()
	>;
};

template <std::size_t I, std::size_t J>
static constexpr void test_swap_types() {
	if constexpr(I == 242 && J == 242) {
		return;
	} else {
		using T = kind_encoding<I>::type;
		using E = kind_encoding<J>::type;
		using R = tim::Result<T, E>;
		constexpr bool has_swap = std::conjunction_v<
			std::is_swappable<T>,
			std::is_swappable<E>,
			std::is_move_constructible<T>,
			std::is_move_constructible<E>,
			std::disjunction<
				std::is_nothrow_move_constructible<T>,
				std::is_nothrow_move_constructible<E>
			>
		>;
		constexpr bool has_nothrow_swap = std::conjunction_v<
			std::is_nothrow_swappable<T>,
			std::is_nothrow_swappable<E>,
			std::is_nothrow_move_constructible<T>,
			std::is_nothrow_move_constructible<E>
		>;
		constexpr bool has_move_assign = std::conjunction_v<
			std::is_move_assignable<T>,
			std::is_move_constructible<T>,
			std::is_move_assignable<E>,
			std::is_move_constructible<E>,
			std::disjunction<
				std::is_nothrow_move_constructible<T>,
				std::is_nothrow_move_constructible<E>
			>
		>;
		constexpr bool has_move_constructor = std::conjunction_v<
			std::is_move_constructible<T>,
			std::is_move_constructible<E>
		>;
		static_assert(is_member_swappable_v<R> == has_swap);
		static_assert(is_non_member_swappable_v<R> == has_swap);
		static_assert(std::is_move_assignable_v<R> == has_move_assign);
		static_assert(std::is_move_constructible_v<R> == has_move_constructor);
		static_assert(std::is_swappable_v<R> == (has_move_constructor && has_move_assign));
		static_assert(std::is_swappable_v<R> == (has_move_constructor && has_move_assign));
		static_assert(std::conditional_t<
			is_member_swappable_v<R>,
			std::bool_constant<(std::is_nothrow_swappable_v<R> == has_nothrow_swap)>,
			std::true_type
		>::value);
		if constexpr(J == 242) {
			return test_swap_types<I + 1, 0>();
		} else {
			return test_swap_types<I, J + 1>();
		}
	}
}

} /* namespace types */

void test_swap_sfinae() {
	test_swap_types<0, 0>();
}

void test_swap_noexcept() {
	{
		using V = tim::Result<int, NothrowMoveable>;
		static_assert(std::is_swappable_v<V>);
		static_assert(is_std_swappable_v<V>);
		static_assert(is_non_member_swappable_v<V>);
		static_assert(is_member_swappable_v<V>);
		static_assert(std::is_nothrow_swappable_v<V>, "");
		V v1, v2;
		v1.swap(v2);
		swap(v1, v2);
	}
	{
		using V = tim::Result<NothrowMoveable, int>;
		static_assert(std::is_swappable_v<V>);
		static_assert(is_std_swappable_v<V>);
		static_assert(is_non_member_swappable_v<V>);
		static_assert(is_member_swappable_v<V>);
		static_assert(std::is_nothrow_swappable_v<V>, "");
		V v1, v2;
		v1.swap(v2);
		swap(v1, v2);
	}
	{
		using V = tim::Result<int, NothrowMoveCtor>;
		static_assert(std::is_swappable_v<V>);
		static_assert(is_std_swappable_v<V>);
		static_assert(!is_non_member_swappable_v<V>);
		static_assert(!is_member_swappable_v<V>);
		static_assert(std::is_nothrow_swappable_v<V>, "");
		V v1, v2;
		v1.swap(v2);
		swap(v1, v2);
	}
	{
		using V = tim::Result<NothrowMoveCtor, int>;
		static_assert(std::is_swappable_v<V>);
		static_assert(is_std_swappable_v<V>);
		static_assert(!is_non_member_swappable_v<V>);
		static_assert(!is_member_swappable_v<V>);
		static_assert(std::is_nothrow_swappable_v<V>, "");
		V v1, v2;
		v1.swap(v2);
		swap(v1, v2);
	}
	{
		using V = tim::Result<int, ThrowingTypeWithNothrowSwap>;
		static_assert(std::is_swappable_v<V>);
		static_assert(is_std_swappable_v<V>);
		static_assert(is_non_member_swappable_v<V>);
		static_assert(is_member_swappable_v<V>);
		static_assert(!std::is_nothrow_swappable_v<V>, "");
		// instantiate swap
		V v1, v2;
		v1.swap(v2);
		swap(v1, v2);
	}
	{
		using V = tim::Result<ThrowingTypeWithNothrowSwap, int>;
		static_assert(std::is_swappable_v<V>);
		static_assert(is_std_swappable_v<V>);
		static_assert(is_non_member_swappable_v<V>);
		static_assert(is_member_swappable_v<V>);
		static_assert(!std::is_nothrow_swappable_v<V>, "");
		// instantiate swap
		V v1, v2;
		v1.swap(v2);
		swap(v1, v2);
	}
	{
		using V = tim::Result<int, ThrowingMoveAssignNothrowMoveCtor>;
		static_assert(std::is_swappable_v<V>);
		static_assert(is_std_swappable_v<V>);
		static_assert(is_non_member_swappable_v<V>);
		static_assert(is_member_swappable_v<V>);
		static_assert(!std::is_nothrow_swappable_v<V>, "");
		// instantiate swap
		V v1, v2;
		v1.swap(v2);
		swap(v1, v2);
	}
	{
		using V = tim::Result<ThrowingMoveAssignNothrowMoveCtor, int>;
		static_assert(std::is_swappable_v<V>);
		static_assert(is_std_swappable_v<V>);
		static_assert(is_non_member_swappable_v<V>);
		static_assert(is_member_swappable_v<V>);
		static_assert(!std::is_nothrow_swappable_v<V>, "");
		// instantiate swap
		V v1, v2;
		v1.swap(v2);
		swap(v1, v2);
	}
	{
		using V = tim::Result<int, ThrowingMoveAssignNothrowMoveCtorWithSwap>;
		static_assert(std::is_swappable_v<V>);
		static_assert(is_std_swappable_v<V>);
		static_assert(is_non_member_swappable_v<V>);
		static_assert(is_member_swappable_v<V>);
		static_assert(!std::is_nothrow_swappable_v<V>, "");
		// instantiate swap
		V v1, v2;
		v1.swap(v2);
		swap(v1, v2);
	}
	{
		using V = tim::Result<ThrowingMoveAssignNothrowMoveCtorWithSwap, int>;
		static_assert(std::is_swappable_v<V>);
		static_assert(is_std_swappable_v<V>);
		static_assert(is_non_member_swappable_v<V>);
		static_assert(is_member_swappable_v<V>);
		static_assert(!std::is_nothrow_swappable_v<V>, "");
		// instantiate swap
		V v1, v2;
		v1.swap(v2);
		swap(v1, v2);
	}
	{
		using V = tim::Result<int, NotMoveAssignableWithSwap>;
		static_assert(std::is_swappable_v<V>);
		static_assert(is_std_swappable_v<V>);
		static_assert(is_non_member_swappable_v<V>);
		static_assert(is_member_swappable_v<V>);
		static_assert(!std::is_nothrow_swappable_v<V>, "");
		// instantiate swap
		V v1, v2;
		v1.swap(v2);
		swap(v1, v2);
	}
	{
		using V = tim::Result<NotMoveAssignableWithSwap, int>;
		static_assert(std::is_swappable_v<V>);
		static_assert(is_std_swappable_v<V>);
		static_assert(is_non_member_swappable_v<V>);
		static_assert(is_member_swappable_v<V>);
		static_assert(!std::is_nothrow_swappable_v<V>, "");
		// instantiate swap
		V v1, v2;
		v1.swap(v2);
		swap(v1, v2);
	}
	{
		using V = tim::Result<int, NotSwappable>;
		static_assert(std::is_swappable_v<V>);
		static_assert(is_std_swappable_v<V>);
		static_assert(!is_non_member_swappable_v<V>);
		static_assert(!is_member_swappable_v<V>);
		static_assert(!std::is_nothrow_swappable_v<V>, "");
		V v1, v2;
		std::swap(v1, v2);
	}
}

} /* namespace swap_test_namespace */

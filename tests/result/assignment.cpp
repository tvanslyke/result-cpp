#include "catch.hpp"
#include <cassert>
#include "tim/result/Result.hpp"

#include "support/variant_test_helpers.h"
TEST_CASE("Simple assignment", "[assignment.simple]") {
	tim::Result<int, int> e1 = 42;
	tim::Result<int, int> e2 = 17;
	tim::Result<int, int> e3 = 21;
	tim::Result<int, int> e4 = tim::make_error(42);
	tim::Result<int, int> e5 = tim::make_error(17);
	tim::Result<int, int> e6 = tim::make_error(21);

	e1 = e2;
	REQUIRE(e1);
	REQUIRE((*e1) == 17);
	REQUIRE(e2);
	REQUIRE((*e2) == 17);

	e1 = std::move(e2);
	REQUIRE(e1);
	REQUIRE((*e1) == 17);
	REQUIRE(e2);
	REQUIRE((*e2) == 17);

	e1 = 42;
	REQUIRE(e1);
	REQUIRE((*e1) == 42);

	auto unex = tim::make_error(12);
	e1 = unex;
	REQUIRE(!e1);
	REQUIRE(e1.error() == 12);

	e1 = tim::make_error(42);
	REQUIRE(!e1);
	REQUIRE(e1.error() == 42);

	e1 = e3;
	REQUIRE(e1);
	REQUIRE((*e1) == 21);

	e4 = e5;
	REQUIRE(!e4);
	REQUIRE(e4.error() == 17);

	e4 = std::move(e6);
	REQUIRE(!e4);
	REQUIRE(e4.error() == 21);

	e4 = e1;
	REQUIRE(e4);
	REQUIRE((*e4) == 21);
}

TEST_CASE("Assignment deletion", "[assignment.deletion]") {
	struct has_all {
		has_all() = default;
		has_all(const has_all &) = default;
		has_all(has_all &&) noexcept = default;
		has_all &operator=(const has_all &) = default;
	};

	tim::Result<has_all, has_all> e1 = {};
	tim::Result<has_all, has_all> e2 = {};
	e1 = e2;

	struct except_move {
		except_move() = default;
		except_move(const except_move &) = default;
		except_move(except_move &&) noexcept(false){};
		except_move &operator=(const except_move &) = default;
	};
	tim::Result<except_move, except_move> e3 = {};
	tim::Result<except_move, except_move> e4 = {};
	// e3 = e4; should not compile
}

namespace copy_assignment {

struct NoCopy {
	NoCopy(const NoCopy &) = delete;
	NoCopy &operator=(const NoCopy &) = default;
};

struct CopyOnly {
	CopyOnly(const CopyOnly &) = default;
	CopyOnly(CopyOnly &&) = delete;
	CopyOnly &operator=(const CopyOnly &) = default;
	CopyOnly &operator=(CopyOnly &&) = delete;
};

struct MoveOnly {
	MoveOnly(const MoveOnly &) = delete;
	MoveOnly(MoveOnly &&) = default;
	MoveOnly &operator=(const MoveOnly &) = default;
};

struct MoveOnlyNT {
	MoveOnlyNT(const MoveOnlyNT &) = delete;
	MoveOnlyNT(MoveOnlyNT &&) {}
	MoveOnlyNT &operator=(const MoveOnlyNT &) = default;
};

struct CopyAssign {
	static int alive;
	static int copy_construct;
	static int copy_assign;
	static int move_construct;
	static int move_assign;
	static void reset() {
		copy_construct = copy_assign = move_construct = move_assign = alive = 0;
	}
	CopyAssign(int v) : value(v) { ++alive; }
	CopyAssign(const CopyAssign &o) : value(o.value) {
		++alive;
		++copy_construct;
	}
	CopyAssign(CopyAssign &&o) noexcept : value(o.value) {
		o.value = -1;
		++alive;
		++move_construct;
	}
	CopyAssign &operator=(const CopyAssign &o) {
		value = o.value;
		++copy_assign;
		return *this;
	}
	CopyAssign &operator=(CopyAssign &&o) noexcept {
		value = o.value;
		o.value = -1;
		++move_assign;
		return *this;
	}
	~CopyAssign() { --alive; }
	int value;
};

int CopyAssign::alive = 0;
int CopyAssign::copy_construct = 0;
int CopyAssign::copy_assign = 0;
int CopyAssign::move_construct = 0;
int CopyAssign::move_assign = 0;

struct CopyMaybeThrows {
	CopyMaybeThrows(const CopyMaybeThrows &);
	CopyMaybeThrows &operator=(const CopyMaybeThrows &);
};
struct CopyDoesThrow {
	CopyDoesThrow(const CopyDoesThrow &) noexcept(false);
	CopyDoesThrow &operator=(const CopyDoesThrow &) noexcept(false);
};


struct NTCopyAssign {
	constexpr NTCopyAssign(int v) : value(v) {}
	NTCopyAssign(const NTCopyAssign &) = default;
	NTCopyAssign(NTCopyAssign &&) = default;
	NTCopyAssign &operator=(const NTCopyAssign &that) {
		value = that.value;
		return *this;
	};
	NTCopyAssign &operator=(NTCopyAssign &&) = delete;
	int value;
};

static_assert(!std::is_trivially_copy_assignable<NTCopyAssign>::value, "");
static_assert(std::is_copy_assignable<NTCopyAssign>::value, "");

struct TCopyAssign {
	constexpr TCopyAssign(int v) : value(v) {}
	TCopyAssign(const TCopyAssign &) = default;
	TCopyAssign(TCopyAssign &&) = default;
	TCopyAssign &operator=(const TCopyAssign &) = default;
	TCopyAssign &operator=(TCopyAssign &&) = delete;
	int value;
};

static_assert(std::is_trivially_copy_assignable<TCopyAssign>::value, "");

struct TCopyAssignNTMoveAssign {
	constexpr TCopyAssignNTMoveAssign(int v) : value(v) {}
	TCopyAssignNTMoveAssign(const TCopyAssignNTMoveAssign &) = default;
	TCopyAssignNTMoveAssign(TCopyAssignNTMoveAssign &&) = default;
	TCopyAssignNTMoveAssign &operator=(const TCopyAssignNTMoveAssign &) = default;
	TCopyAssignNTMoveAssign &operator=(TCopyAssignNTMoveAssign &&that) {
		value = that.value;
		that.value = -1;
		return *this;
	}
	int value;
};

static_assert(std::is_trivially_copy_assignable_v<TCopyAssignNTMoveAssign>, "");

struct CopyThrows {
	CopyThrows() = default;
	CopyThrows(const CopyThrows &) { throw 42; }
	CopyThrows &operator=(const CopyThrows &) { throw 42; }
};

struct CopyCannotThrow {
	static int alive;
	CopyCannotThrow() { ++alive; }
	CopyCannotThrow(const CopyCannotThrow &) noexcept { ++alive; }
	CopyCannotThrow(CopyCannotThrow &&) noexcept { REQUIRE(false); }
	CopyCannotThrow &operator=(const CopyCannotThrow &) noexcept = default;
	CopyCannotThrow &operator=(CopyCannotThrow &&) noexcept { REQUIRE(false); return *this; }
};

int CopyCannotThrow::alive = 0;

// struct MoveThrows {
// 	static int alive;
// 	MoveThrows() { ++alive; }
// 	MoveThrows(const MoveThrows &) { ++alive; }
// 	MoveThrows(MoveThrows &&) { throw 42; }
// 	MoveThrows &operator=(const MoveThrows &) { return *this; }
// 	MoveThrows &operator=(MoveThrows &&) { throw 42; }
// 	~MoveThrows() { --alive; }
// };
// 
// int MoveThrows::alive = 0;

TEST_CASE("Copy assignment not noexcept", "[assignment.copy]") {
	{
		using V = tim::Result<CopyMaybeThrows, int>;
		static_assert(!std::is_nothrow_copy_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, CopyDoesThrow>;
		static_assert(!std::is_nothrow_copy_assignable<V>::value, "");
	}
}

TEST_CASE("Copy assignment sfinae", "[assignment.copy]") {
	{
		using V = tim::Result<int, long>;
		static_assert(std::is_copy_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, CopyOnly>;
		static_assert(std::is_copy_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, NoCopy>;
		static_assert(!std::is_copy_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, MoveOnly>;
		static_assert(!std::is_copy_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, MoveOnlyNT>;
		static_assert(!std::is_copy_assignable<V>::value, "");
	}

	{
		using V = tim::Result<long, int>;
		static_assert(std::is_copy_assignable<V>::value, "");
	}
	{
		using V = tim::Result<CopyOnly, int>;
		static_assert(std::is_copy_assignable<V>::value, "");
	}
	{
		using V = tim::Result<NoCopy, int>;
		static_assert(!std::is_copy_assignable<V>::value, "");
	}
	{
		using V = tim::Result<MoveOnly, int>;
		static_assert(!std::is_copy_assignable<V>::value, "");
	}
	{
		using V = tim::Result<MoveOnlyNT, int>;
		static_assert(!std::is_copy_assignable<V>::value, "");
	}

	{
		using V = tim::Result<int, long>;
		static_assert(std::is_trivially_copy_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, NTCopyAssign>;
		static_assert(!std::is_trivially_copy_assignable<V>::value, "");
		static_assert(std::is_copy_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, TCopyAssign>;
		static_assert(std::is_trivially_copy_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, TCopyAssignNTMoveAssign>;
		static_assert(std::is_trivially_copy_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, CopyOnly>;
		static_assert(std::is_trivially_copy_assignable<V>::value, "");
	}
	{
		using V = tim::Result<long, int>;
		static_assert(std::is_trivially_copy_assignable<V>::value, "");
	}
	{
		using V = tim::Result<NTCopyAssign, int>;
		static_assert(!std::is_trivially_copy_assignable<V>::value, "");
		static_assert(std::is_copy_assignable<V>::value, "");
	}
	{
		using V = tim::Result<TCopyAssign, int>;
		static_assert(std::is_trivially_copy_assignable<V>::value, "");
	}
	{
		using V = tim::Result<TCopyAssignNTMoveAssign, int>;
		static_assert(std::is_trivially_copy_assignable<V>::value, "");
	}
	{
		using V = tim::Result<CopyOnly, int>;
		static_assert(std::is_trivially_copy_assignable<V>::value, "");
	}
}

template <typename T> struct ResultT { bool has_value; T value; };

TEST_CASE("Copy assignment same values", "[assignment.copy]") {
	{
		using V = tim::Result<int, int>;
		V v1(43);
		V v2(42);
		V &vref = (v1 = v2);
		REQUIRE(&vref == &v1);
		REQUIRE(v1.has_value());
		REQUIRE((*v1) == 42);
	}
	{
		using V = tim::Result<int, long>;
		V v1(tim::Error(43l));
		V v2(tim::Error(42l));
		V &vref = (v1 = v2);
		REQUIRE(&vref == &v1);
		REQUIRE(!v1.has_value());
		REQUIRE(v1.error() == 42);
	}
	{
		using V = tim::Result<int, CopyAssign>;
		V v1(tim::in_place_error, 43);
		V v2(tim::in_place_error, 42);
		CopyAssign::reset();
		V &vref = (v1 = v2);
		REQUIRE(&vref == &v1);
		REQUIRE(!v1.has_value());
		REQUIRE(v1.error().value == 42);
		REQUIRE(CopyAssign::copy_construct == 0);
		REQUIRE(CopyAssign::move_construct == 0);
		REQUIRE(CopyAssign::copy_assign == 1);
	}

	{
		struct {
			constexpr ResultT<int> operator()() const {
				using V = tim::Result<int, int>;
				V v(43);
				V v2(42);
				v = v2;
				return {v.has_value(), (*v)};
			}
		} test;
		constexpr auto result = test();
		static_assert(result.has_value, "");
		static_assert(result.value == 42, "");
	}
	{
		struct {
			constexpr ResultT<long> operator()() const {
				using V = tim::Result<int, long>;
				V v(tim::Error(43l));
				V v2(tim::Error(42l));
				v = v2;
				return {v.has_value(), v.error()};
			}
		} test;
		constexpr auto result = test();
		static_assert(!result.has_value, "");
		static_assert(result.value == 42l, "");
	}
	{
		struct {
			constexpr ResultT<int> operator()() const {
				using V = tim::Result<int, TCopyAssign>;
				V v(tim::in_place_error, 43);
				V v2(tim::in_place_error, 42);
				v = v2;
				return {v.has_value(), v.error().value};
			}
		} test;
		constexpr auto result = test();
		static_assert(!result.has_value, "");
		static_assert(result.value == 42, "");
	}
	{
		struct {
			constexpr ResultT<int> operator()() const {
				using V = tim::Result<int, TCopyAssignNTMoveAssign>;
				V v(tim::in_place_error, 43);
				V v2(tim::in_place_error, 42);
				v = v2;
				return {v.has_value(), v.error().value};
			}
		} test;
		constexpr auto result = test();
		static_assert(!result.has_value, "");
		static_assert(result.value == 42, "");
	}
}

TEST_CASE("Copy assignment different values", "[assignment.copy]") {
	{
		using V = tim::Result<int, long>;
		V v1(43);
		V v2(tim::in_place_error, 42l);
		V &vref = (v1 = v2);
		REQUIRE(&vref == &v1);
		REQUIRE(!v1.has_value());
		REQUIRE(v1.error() == 42);
	}
	{
		using V = tim::Result<int, CopyAssign>;
		CopyAssign::reset();
		V v1(tim::in_place, 43);
		V v2(tim::in_place_error, 42);
		REQUIRE(CopyAssign::copy_construct == 0);
		REQUIRE(CopyAssign::move_construct == 0);
		REQUIRE(CopyAssign::alive == 1);
		V &vref = (v1 = v2);
		REQUIRE(&vref == &v1);
		REQUIRE(!v1.has_value());
		REQUIRE(v1.error().value == 42);
		REQUIRE(CopyAssign::alive == 2);
		REQUIRE(CopyAssign::copy_construct == 1);
		REQUIRE(CopyAssign::move_construct == 1);
		REQUIRE(CopyAssign::copy_assign == 0);
	}
	{
		using V = tim::Result<std::string, MoveThrows>;
		V v1(tim::in_place, "hello");
		V v2(tim::in_place_error);
		REQUIRE(MoveThrows::alive == 1);
		// Test that copy construction is used directly if move construction may throw.
		try {
			v1 = v2;
			REQUIRE(!v1.has_value());
			REQUIRE(!v2.has_value());
			REQUIRE(MoveThrows::alive == 2);
		} catch(...) {
			REQUIRE(false);
		}
	}
	{
		// Test that direct copy construction is preferred when it cannot throw.
		using V = tim::Result<std::string, CopyCannotThrow>;
		V v1(tim::in_place, "hello");
		V v2(tim::in_place_error);
		REQUIRE(CopyCannotThrow::alive == 1);
		v1 = v2;
		REQUIRE(!v1.has_value());
		REQUIRE(!v2.has_value());
		REQUIRE(CopyCannotThrow::alive == 2);
	}
	{
		using V = tim::Result<std::string, CopyThrows>;
		V v1(tim::in_place_error);
		V v2(tim::in_place, "hello");
		try {
			V &vref = (v1 = v2);
			REQUIRE(&vref == &v1);
			REQUIRE(v1.has_value());
			REQUIRE((*v1) == "hello");
			REQUIRE(v2.has_value());
			REQUIRE((*v2) == "hello");
		} catch(...) {
			REQUIRE(false);
		}
	}
	{
		using V = tim::Result<std::string, MoveThrows>;
		V v1(tim::in_place_error);
		V v2(tim::in_place, "hello");
		try {
			V &vref = (v1 = v2);
			REQUIRE(&vref == &v1);
			REQUIRE(v1.has_value());
			REQUIRE((*v1) == "hello");
			REQUIRE(v2.has_value());
			REQUIRE((*v2) == "hello");
		} catch(...) {
			REQUIRE(false);
		}
	}
	{
		using V = tim::Result<CopyAssign, int>;
		CopyAssign::reset();
		V v1(tim::in_place_error, 43);
		V v2(tim::in_place, 42);
		REQUIRE(CopyAssign::copy_construct == 0);
		REQUIRE(CopyAssign::move_construct == 0);
		REQUIRE(CopyAssign::alive == 1);
		V &vref = (v1 = v2);
		REQUIRE(&vref == &v1);
		REQUIRE(v1.has_value());
		REQUIRE((*v1).value == 42);
		REQUIRE(CopyAssign::alive == 2);
		REQUIRE(CopyAssign::copy_construct == 1);
		REQUIRE(CopyAssign::move_construct == 1);
		REQUIRE(CopyAssign::copy_assign == 0);
	}

	{
		MoveThrows::alive = 0;
		using V = tim::Result<MoveThrows, std::string>;
		V v1(tim::in_place_error, "hello");
		V v2(tim::in_place);
		REQUIRE(MoveThrows::alive == 1);
		// Test that copy construction is used directly if move construction may throw.
		try{ 
			v1 = v2;
			REQUIRE(v1.has_value());
			REQUIRE(v2.has_value());
			REQUIRE(MoveThrows::alive == 2);
		} catch(...) {
			REQUIRE(false);
		}
	}
	{
		CopyCannotThrow::alive = 0;
		// Test that direct copy construction is preferred when it cannot throw.
		using V = tim::Result<CopyCannotThrow, std::string>;
		REQUIRE(CopyCannotThrow::alive == 0);
		V v1(tim::in_place_error, "hello");
		V v2(tim::in_place);
		REQUIRE(CopyCannotThrow::alive == 1);
		v1 = v2;
		REQUIRE(v1.has_value());
		REQUIRE(v2.has_value());
		REQUIRE(CopyCannotThrow::alive == 2);
	}
	{
		using V = tim::Result<CopyThrows, std::string>;
		V v1(tim::in_place);
		V v2(tim::in_place_error, "hello");
		try {
			V &vref = (v1 = v2);
			REQUIRE(&vref == &v1);
			REQUIRE(!v1.has_value());
			REQUIRE(v1.error() == "hello");
			REQUIRE(!v2.has_value());
			REQUIRE(v2.error() == "hello");
		} catch(...) {
			REQUIRE(false);
		}
	}
	{
		using V = tim::Result<MoveThrows, std::string>;
		V v1(tim::in_place);
		V v2(tim::in_place_error, "hello");
		try {
			V &vref = (v1 = v2);
			REQUIRE(&vref == &v1);
			REQUIRE(!v1.has_value());
			REQUIRE(v1.error() == "hello");
			REQUIRE(!v2.has_value());
			REQUIRE(v2.error() == "hello");
		} catch(...) {
			REQUIRE(false);
		}
	}

	{
		struct {
			constexpr ResultT<long> operator()() const {
				using V = tim::Result<int, long>;
				V v(43);
				V v2(tim::Error(42l));
				v = v2;
				return {v.has_value(), v.error()};
			}
		} test;
		constexpr auto result = test();
		static_assert(!result.has_value, "");
		static_assert(result.value == 42l, "");
	}
	{
		struct {
			constexpr ResultT<int> operator()() const {
				using V = tim::Result<int, TCopyAssign>;
				V v(tim::in_place, 43);
				V v2(tim::in_place_error, 42);
				v = v2;
				return {v.has_value(), v.error().value};
			}
		} test;
		constexpr auto result = test();
		static_assert(!result.has_value, "");
		static_assert(result.value == 42, "");
	}
}

template <bool HasVal, class ValueType>
constexpr bool test_constexpr_assign_imp(tim::Result<long, void*>&& v, ValueType&& new_value)
{
	const tim::Result<long, void*> cp(std::forward<ValueType>(new_value));
	v = cp;
	return v.has_value() == HasVal
		&& (HasVal ? (*v) == (*cp) : v.error() == cp.error());
}
 
TEST_CASE("Constexpr copy assignment", "[assignment.copy]") {
	using V = tim::Result<long, void*>;
	static_assert(std::is_trivially_copyable<V>::value, "");
	static_assert(std::is_trivially_copy_assignable<V>::value, "");
	static_assert(test_constexpr_assign_imp<true>(V(42l), 101l), "");
	static_assert(test_constexpr_assign_imp<true>(V(tim::Error(nullptr)), 101l), "");
	static_assert(test_constexpr_assign_imp<false>(V(42l), tim::Error(nullptr)), "");
}

} /* namespace copy_assignment */

namespace move_assignment {

struct NoCopy {
	NoCopy(const NoCopy &) = delete;
	NoCopy &operator=(const NoCopy &) = default;
};

struct CopyOnly {
	CopyOnly(const CopyOnly &) = default;
	CopyOnly(CopyOnly &&) = delete;
	CopyOnly &operator=(const CopyOnly &) = default;
	CopyOnly &operator=(CopyOnly &&) = delete;
};

struct MoveOnly {
	MoveOnly(const MoveOnly &) = delete;
	MoveOnly(MoveOnly &&) = default;
	MoveOnly &operator=(const MoveOnly &) = delete;
	MoveOnly &operator=(MoveOnly &&) = default;
};

struct MoveOnlyNT {
	MoveOnlyNT(const MoveOnlyNT &) = delete;
	MoveOnlyNT(MoveOnlyNT &&) {}
	MoveOnlyNT &operator=(const MoveOnlyNT &) = delete;
	MoveOnlyNT &operator=(MoveOnlyNT &&) = default;
};

struct MoveOnlyOddNothrow {
	MoveOnlyOddNothrow(MoveOnlyOddNothrow &&) noexcept(false) {}
	MoveOnlyOddNothrow(const MoveOnlyOddNothrow &) = delete;
	MoveOnlyOddNothrow &operator=(MoveOnlyOddNothrow &&) noexcept = default;
	MoveOnlyOddNothrow &operator=(const MoveOnlyOddNothrow &) = delete;
};

struct MoveAssignOnly {
	MoveAssignOnly(MoveAssignOnly &&) = delete;
	MoveAssignOnly &operator=(MoveAssignOnly &&) = default;
};

struct MoveAssign {
	static int move_construct;
	static int move_assign;
	static void reset() { move_construct = move_assign = 0; }
	MoveAssign(int v) : value(v) {}
	MoveAssign(MoveAssign &&o) : value(o.value) {
		++move_construct;
		o.value = -1;
	}
	MoveAssign &operator=(MoveAssign &&o) {
		value = o.value;
		++move_assign;
		o.value = -1;
		return *this;
	}
	int value;
};

int MoveAssign::move_construct = 0;
int MoveAssign::move_assign = 0;

struct NTMoveAssign {
	constexpr NTMoveAssign(int v) : value(v) {}
	NTMoveAssign(const NTMoveAssign &) = default;
	NTMoveAssign(NTMoveAssign &&) = default;
	NTMoveAssign &operator=(const NTMoveAssign &that) = default;
	NTMoveAssign &operator=(NTMoveAssign &&that) {
		value = that.value;
		that.value = -1;
		return *this;
	};
	int value;
};

static_assert(!std::is_trivially_move_assignable<NTMoveAssign>::value, "");
static_assert(std::is_move_assignable<NTMoveAssign>::value, "");

struct TMoveAssign {
	constexpr TMoveAssign(int v) : value(v) {}
	TMoveAssign(const TMoveAssign &) = delete;
	TMoveAssign(TMoveAssign &&) = default;
	TMoveAssign &operator=(const TMoveAssign &) = delete;
	TMoveAssign &operator=(TMoveAssign &&) = default;
	int value;
};

static_assert(std::is_trivially_move_assignable<TMoveAssign>::value, "");

struct TMoveAssignNTCopyAssign {
	constexpr TMoveAssignNTCopyAssign(int v) : value(v) {}
	TMoveAssignNTCopyAssign(const TMoveAssignNTCopyAssign &) = default;
	TMoveAssignNTCopyAssign(TMoveAssignNTCopyAssign &&) = default;
	TMoveAssignNTCopyAssign &operator=(const TMoveAssignNTCopyAssign &that) {
		value = that.value;
		return *this;
	}
	TMoveAssignNTCopyAssign &operator=(TMoveAssignNTCopyAssign &&) = default;
	int value;
};


static_assert(std::is_trivially_move_assignable_v<TMoveAssignNTCopyAssign>, "");

struct TrivialCopyNontrivialMove {
	TrivialCopyNontrivialMove(TrivialCopyNontrivialMove const&) = default;
	TrivialCopyNontrivialMove(TrivialCopyNontrivialMove&&) noexcept {}
	TrivialCopyNontrivialMove& operator=(TrivialCopyNontrivialMove const&) = default;
	TrivialCopyNontrivialMove& operator=(TrivialCopyNontrivialMove&&) noexcept {
		return *this;
	}
};

static_assert(std::is_trivially_copy_assignable_v<TrivialCopyNontrivialMove>, "");
static_assert(!std::is_trivially_move_assignable_v<TrivialCopyNontrivialMove>, "");


TEST_CASE("Move assignment noexcept", "[assignment.move]") {
	{
		using V = tim::Result<int, int>;
		static_assert(std::is_nothrow_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, MoveOnly>;
		static_assert(std::is_nothrow_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, long>;
		static_assert(std::is_nothrow_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, MoveOnly>;
		static_assert(std::is_nothrow_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, MoveOnlyNT>;
		static_assert(!std::is_nothrow_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, MoveOnlyOddNothrow>;
		static_assert(!std::is_nothrow_move_assignable<V>::value, "");
	}

	{
		using V = tim::Result<int, int>;
		static_assert(std::is_nothrow_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<MoveOnly, int>;
		static_assert(std::is_nothrow_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, long>;
		static_assert(std::is_nothrow_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, MoveOnly>;
		static_assert(std::is_nothrow_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<MoveOnlyNT, int>;
		static_assert(!std::is_nothrow_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<MoveOnlyOddNothrow, int>;
		static_assert(!std::is_nothrow_move_assignable<V>::value, "");
	}
}

TEST_CASE("Move assignment sfinae", "[assignment.move]") {
	{
		using V = tim::Result<int, long>;
		static_assert(std::is_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, CopyOnly>;
		static_assert(std::is_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, NoCopy>;
		static_assert(!std::is_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, MoveOnly>;
		static_assert(std::is_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, MoveOnlyNT>;
		static_assert(std::is_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, MoveAssignOnly>;
		static_assert(!std::is_move_assignable<V>::value, "");
	}

	{
		using V = tim::Result<long, long>;
		static_assert(std::is_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<CopyOnly, int>;
		static_assert(std::is_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<NoCopy, int>;
		static_assert(!std::is_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<MoveOnly, int>;
		static_assert(std::is_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<MoveOnlyNT, int>;
		static_assert(std::is_move_assignable<V>::value, "");
	}
	{
		// variant only provides move assignment when the types also provide
		// a move constructor.
		using V = tim::Result<MoveAssignOnly, int>;
		static_assert(!std::is_move_assignable<V>::value, "");
	}

	{
		using V = tim::Result<int, long>;
		static_assert(std::is_trivially_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, NTMoveAssign>;
		static_assert(!std::is_trivially_move_assignable<V>::value, "");
		static_assert(std::is_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, TMoveAssign>;
		static_assert(std::is_trivially_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, TMoveAssignNTCopyAssign>;
		static_assert(std::is_trivially_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, TrivialCopyNontrivialMove>;
		static_assert(!std::is_trivially_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<int, CopyOnly>;
		static_assert(std::is_trivially_move_assignable<V>::value, "");
	}

	{
		using V = tim::Result<long, int>;
		static_assert(std::is_trivially_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<NTMoveAssign, int>;
		static_assert(!std::is_trivially_move_assignable<V>::value, "");
		static_assert(std::is_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<TMoveAssign, int>;
		static_assert(std::is_trivially_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<TMoveAssignNTCopyAssign, int>;
		static_assert(std::is_trivially_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<TrivialCopyNontrivialMove, int>;
		static_assert(!std::is_trivially_move_assignable<V>::value, "");
	}
	{
		using V = tim::Result<CopyOnly, int>;
		static_assert(std::is_trivially_move_assignable<V>::value, "");
	}
}

template <typename T> struct ResultT { size_t has_value; T value; };

TEST_CASE("Move assignment same values", "[assignment.move]") {
	{
		using V = tim::Result<int, int>;
		V v1(43);
		V v2(42);
		V &vref = (v1 = std::move(v2));
		REQUIRE(&vref == &v1);
		REQUIRE(v1.has_value());
		REQUIRE(v1.value() == 42);
	}
	{
		using V = tim::Result<int, long>;
		V v1(tim::Error(43l));
		V v2(tim::Error(42l));
		V &vref = (v1 = std::move(v2));
		REQUIRE(&vref == &v1);
		REQUIRE(!v1.has_value());
		REQUIRE(v1.error() == 42);
	}
	{
		using V = tim::Result<int, MoveAssign>;
		V v1(tim::in_place_error, 43);
		V v2(tim::in_place_error, 42);
		MoveAssign::reset();
		V &vref = (v1 = std::move(v2));
		REQUIRE(&vref == &v1);
		REQUIRE(!v1.has_value());
		REQUIRE(v1.error().value == 42);
		REQUIRE(MoveAssign::move_construct == 0);
		REQUIRE(MoveAssign::move_assign == 1);
	}

	{
		struct {
			constexpr ResultT<int> operator()() const {
				using V = tim::Result<int, int>;
				V v(43);
				V v2(42);
				v = std::move(v2);
				return {v.has_value(), v.value()};
			}
		} test;
		constexpr auto result = test();
		static_assert(result.has_value == true, "");
		static_assert(result.value == 42, "");
	}
	{
		struct {
			constexpr ResultT<long> operator()() const {
				using V = tim::Result<int, long>;
				V v(tim::Error(43l));
				V v2(tim::Error(42l));
				v = std::move(v2);
				return {v.has_value(), v.error()};
			}
		} test;
		constexpr auto result = test();
		static_assert(result.has_value == false, "");
		static_assert(result.value == 42l, "");
	}
	{
		struct {
			constexpr ResultT<int> operator()() const {
				using V = tim::Result<int, TMoveAssign>;
				V v(tim::in_place_error, 43);
				V v2(tim::in_place_error, 42);
				v = std::move(v2);
				return {v.has_value(), v.error().value};
			}
		} test;
		constexpr auto result = test();
		static_assert(result.has_value == false, "");
		static_assert(result.value == 42, "");
	}
}

TEST_CASE("Move assignment different values", "[assignment.move]") {
	{
		using V = tim::Result<int, long>;
		V v1(43);
		V v2(tim::Error(42l));
		V &vref = (v1 = std::move(v2));
		REQUIRE(&vref == &v1);
		REQUIRE(!v1.has_value());
		REQUIRE(v1.error() == 42);
	}
	{
		using V = tim::Result<int, MoveAssign>;
		V v1(tim::in_place, 43);
		V v2(tim::in_place_error, 42);
		MoveAssign::reset();
		V &vref = (v1 = std::move(v2));
		REQUIRE(&vref == &v1);
		REQUIRE(!v1.has_value());
		REQUIRE(v1.error().value == 42);
		REQUIRE(MoveAssign::move_construct == 1);
		REQUIRE(MoveAssign::move_assign == 0);
	}{
		using V = tim::Result<MoveAssign, int>;
		V v1(tim::in_place_error, 43);
		V v2(tim::in_place, 42);
		MoveAssign::reset();
		V &vref = (v1 = std::move(v2));
		REQUIRE(&vref == &v1);
		REQUIRE(v1.has_value());
		REQUIRE(v1.value().value == 42);
		REQUIRE(MoveAssign::move_construct == 1);
		REQUIRE(MoveAssign::move_assign == 0);
	}
	{
		struct {
			constexpr ResultT<long> operator()() const {
				using V = tim::Result<int, long>;
				V v(43);
				V v2(tim::Error(42l));
				v = std::move(v2);
				return {v.has_value(), v.error()};
			}
		} test;
		constexpr auto result = test();
		static_assert(result.has_value == false, "");
		static_assert(result.value == 42l, "");
	}
	{
		struct {
			constexpr ResultT<long> operator()() const {
				using V = tim::Result<TMoveAssign, int>;
				V v(tim::in_place_error, 43);
				V v2(tim::in_place, 42);
				v = std::move(v2);
				return {v.has_value(), v.value().value};
			}
		} test;
		constexpr auto result = test();
		static_assert(result.has_value == true, "");
		static_assert(result.value == 42, "");
	}
}

template <bool HasVal, class ValueType>
constexpr bool test_constexpr_assign_imp(tim::Result<long, void*>&& v, ValueType&& new_value)
{
	tim::Result<long, void*> v2(std::forward<ValueType>(new_value));
	const auto cp = v2;
	v = std::move(v2);
	return v.has_value() == HasVal
		&& (HasVal ? (*v) == (*cp) : v.error() == cp.error());
}
 
TEST_CASE("Constexpr move assignment", "[assignment.move]") {
	using V = tim::Result<long, void*>;
	static_assert(std::is_trivially_copyable<V>::value, "");
	static_assert(std::is_trivially_move_assignable<V>::value, "");
	static_assert(test_constexpr_assign_imp<true>(V(42l), 101l), "");
	static_assert(test_constexpr_assign_imp<true>(V(tim::Error(nullptr)), 101l), "");
	static_assert(test_constexpr_assign_imp<false>(V(42l), tim::Error(nullptr)), "");
}

} /* namespace move_assignment */

namespace exception_safety {

struct CopyAndMoveCtorsThrow {
	static constexpr int copied_val = 10;
	static constexpr int moved_val = 20;

	CopyAndMoveCtorsThrow() {}
	CopyAndMoveCtorsThrow(bool cpy, bool mv, int v=-1): throw_on_copy(cpy), throw_on_move(mv), value(v) {}
	CopyAndMoveCtorsThrow(const CopyAndMoveCtorsThrow& o):
		throw_on_copy(o.throw_on_copy),
		throw_on_move(o.throw_on_move),
		value(o.value)
	{
		if(o.throw_on_copy) {
			throw 42;
		}
		o.value = copied_val;
	}
	CopyAndMoveCtorsThrow(CopyAndMoveCtorsThrow&& o):
		throw_on_copy(o.throw_on_copy),
		throw_on_move(o.throw_on_move),
		value(o.value)
	{
		if(o.throw_on_move) {
			throw 42;
		}
		o.value = moved_val;
	}

	CopyAndMoveCtorsThrow& operator=(const CopyAndMoveCtorsThrow& o) noexcept {
		throw_on_copy = o.throw_on_copy;
		throw_on_move = o.throw_on_move;
		value = o.value;
		return *this;
	}

	CopyAndMoveCtorsThrow& operator=(CopyAndMoveCtorsThrow&& o) noexcept {
		throw_on_copy = o.throw_on_copy;
		throw_on_move = o.throw_on_move;
		value = o.value;
		return *this;
	}

	bool throw_on_copy = false;
	bool throw_on_move = false;
	mutable int value = -1;
};

TEST_CASE("Assignment exception safety") {
	{
		tim::Result<CopyAndMoveCtorsThrow, int> r1(tim::in_place, true, true, 0);
		tim::Result<CopyAndMoveCtorsThrow, int> r2(tim::in_place, false, false, 1);
		r2 = r1;
		r2 = std::move(r1);
		r2 = tim::Error(-1);
		try {
			r2 = r1;
			REQUIRE(false);
		} catch (...) {
			REQUIRE(!r2);
			REQUIRE(r2.error() == -1);
			REQUIRE(r1.value().value == 0);
		}
		r2 = tim::Error(-2);
		try {
			r2 = std::move(r1);
			REQUIRE(false);
		} catch (...) {
			REQUIRE(!r2);
			REQUIRE(r2.error() == -2);
			REQUIRE(r1.value().value == 0);
		}
	}
	{
		tim::Result<int, CopyAndMoveCtorsThrow> r1(tim::in_place_error, true, true, 0);
		tim::Result<int, CopyAndMoveCtorsThrow> r2(tim::in_place_error, false, false, 1);
		r2 = r1;
		r2 = std::move(r1);
		r2 = -1;
		try {
			r2 = r1;
			REQUIRE(false);
		} catch (...) {
			REQUIRE(r2);
			REQUIRE(r2.value() == -1);
			REQUIRE(r1.error().value == 0);
		}
		r2 = -2;
		try {
			r2 = std::move(r1);
			REQUIRE(false);
		} catch (...) {
			REQUIRE(r2);
			REQUIRE(r2.value() == -2);
			REQUIRE(r1.error().value == 0);
		}
	}
	static_assert(!std::is_copy_assignable_v<tim::Result<CopyAndMoveCtorsThrow, CopyAndMoveCtorsThrow>>, "");
}

} /* namespace exception_safety */

namespace /* T_assignment */ {

namespace MetaHelpers {

struct Dummy {
	Dummy() = default;
};

struct ThrowsCtorT {
	ThrowsCtorT(int) noexcept(false) {}
	ThrowsCtorT &operator=(int) noexcept { return *this; }
};

struct ThrowsAssignT {
	ThrowsAssignT(int) noexcept {}
	ThrowsAssignT &operator=(int) noexcept(false) { return *this; }
};

struct NoThrowT {
	NoThrowT(int) noexcept {}
	NoThrowT &operator=(int) noexcept { return *this; }
};

} /* namespace MetaHelpers */

namespace RuntimeHelpers {

struct ThrowsCtorT {
	int value;
	ThrowsCtorT() : value(0) {}
	ThrowsCtorT(int) noexcept(false) { throw 42; }
	ThrowsCtorT &operator=(int v) noexcept {
		value = v;
		return *this;
	}
};

struct MoveCrashes {
	int value;
	MoveCrashes(int v = 0) noexcept : value{v} {}
	MoveCrashes(MoveCrashes &&) noexcept { REQUIRE(false); }
	MoveCrashes &operator=(MoveCrashes &&) noexcept { REQUIRE(false); return *this; }
	MoveCrashes &operator=(int v) noexcept {
		value = v;
		return *this;
	}
};

struct ThrowsCtorTandMove {
	int value;
	ThrowsCtorTandMove() : value(0) {}
	ThrowsCtorTandMove(int) noexcept(false) { throw 42; }
	ThrowsCtorTandMove(ThrowsCtorTandMove &&) noexcept(false) { REQUIRE(false); }
	ThrowsCtorTandMove &operator=(int v) noexcept {
		value = v;
		return *this;
	}
};

struct ThrowsAssignT {
	int value;
	ThrowsAssignT() : value(0) {}
	ThrowsAssignT(int v) noexcept : value(v) {}
	ThrowsAssignT &operator=(int) noexcept(false) { throw 42; }
};

struct NoThrowT {
	int value;
	NoThrowT() : value(0) {}
	NoThrowT(int v) noexcept : value(v) {}
	NoThrowT &operator=(int v) noexcept {
		value = v;
		return *this;
	}
};

} /* namespace RuntimeHelpers */

TEST_CASE("T assignment noexcept") {
	using namespace MetaHelpers;
	{
		using V = tim::Result<Dummy, NoThrowT>;
		static_assert(!std::is_nothrow_assignable<V, int>::value, "");
	}
	{
		using V = tim::Result<Dummy, ThrowsCtorT>;
		static_assert(!std::is_nothrow_assignable<V, int>::value, "");
	}
	{
		using V = tim::Result<Dummy, ThrowsAssignT>;
		static_assert(!std::is_nothrow_assignable<V, int>::value, "");
	}
	{
		using V = tim::Result<NoThrowT, Dummy>;
		static_assert(std::is_nothrow_assignable<V, int>::value, "");
	}
	{
		using V = tim::Result<ThrowsCtorT, Dummy>;
		static_assert(!std::is_nothrow_assignable<V, int>::value, "");
	}
	{
		using V = tim::Result<ThrowsAssignT, Dummy>;
		static_assert(!std::is_nothrow_assignable<V, int>::value, "");
	}
}

TEST_CASE("T assignment sfinae") {
	{
		using V = tim::Result<long, long long>;
		static_assert(std::is_assignable<V, int>::value, "");
	}
	{
		using V = tim::Result<std::string, std::string>;
		static_assert(std::is_assignable<V, const char *>::value, "ambiguous");
	}
	{
		using V = tim::Result<std::string, void *>;
		static_assert(!std::is_assignable<V, int>::value, "no matching operator=");
	}
	{
		using V = tim::Result<std::string, float>;
		static_assert(std::is_assignable<V, int>::value == false, "no matching operator=");
	}
	{
		using V = tim::Result<std::unique_ptr<int>, bool>;
		static_assert(!std::is_assignable<V, std::unique_ptr<char>>::value,
									"no explicit bool in operator=");
		struct X {
			operator void*();
		};
		static_assert(!std::is_assignable<V, X>::value,
									"no boolean conversion in operator=");
		static_assert(!std::is_assignable<V, std::false_type>::value,
									"no converted to bool in operator=");
	}
	{
		struct X {};
		struct Y {
			operator X();
		};
		using V = tim::Result<X, int>;
		static_assert(std::is_assignable<V, Y>::value, "regression on user-defined conversions in operator=");
	}
}

TEST_CASE("Basic T assignment") {
	{
		tim::Result<int, int> v(43);
		v = 42;
		REQUIRE(v.has_value() == true);
		REQUIRE(v.value() == 42);
	}
	{
		tim::Result<int, long> v(tim::Error(43l));
		v = 42;
		REQUIRE(v.has_value() == true);
		REQUIRE(v.value() == 42);
		v = tim::Error(43l);
		REQUIRE(v.has_value() == false);
		REQUIRE(v.error() == 43);
	}
	{
		tim::Result<unsigned, long> v;
		v = tim::Error(42);
		REQUIRE(v.has_value() == false);
		REQUIRE(v.error() == 42);
		v = 43u;
		REQUIRE(v.has_value() == true);
		REQUIRE(v.value() == 43);
	}
	{
		tim::Result<std::string, bool> v = tim::Error(true);
		v = "bar";
		REQUIRE(v.has_value() == true);
		REQUIRE(v.value() == "bar");
	}
	{
		tim::Result<bool, std::unique_ptr<int>> v;
		v = tim::Error(nullptr);
		REQUIRE(v.has_value() == false);
		REQUIRE(v.error() == nullptr);
	}
	{
		tim::Result<bool volatile, int> v = 42;
		v = false;
		REQUIRE(v.has_value() == true);
		REQUIRE(!v.value());
		bool lvt = true;
		v = lvt;
		REQUIRE(v.has_value() == true);
		REQUIRE(v.value());
		v = tim::Error(false);
		REQUIRE(v.has_value() == false);
		REQUIRE(!v.error());
	}
}

TEST_CASE("T assignment construction") {
	using namespace RuntimeHelpers;
	{
		using V = tim::Result<std::string, ThrowsCtorT>;
		V v(tim::in_place, "hello");
		try {
			v = tim::Error(42);
			REQUIRE(false);
		} catch (...) { /* ... */
		}
		REQUIRE(v.has_value() == true);
		REQUIRE(v.value() == "hello");
	}
	{
		using V = tim::Result<ThrowsAssignT, std::string>;
		V v(tim::in_place_error, "hello");
		v = 42;
		REQUIRE(v.has_value() == true);
		REQUIRE(v.value().value == 42);
	}
}

TEST_CASE("T assignment performs assignment") {
	using namespace RuntimeHelpers;
	{
		using V = tim::Result<ThrowsCtorT, int>;
		V v;
		v = 42;
		REQUIRE(v.has_value() == true);
		REQUIRE(v.value().value == 42);
	}
	{
		using V = tim::Result<ThrowsCtorT, std::string>;
		V v;
		v = 42;
		REQUIRE(v.has_value() == true);
		REQUIRE(v.value().value == 42);
	}
	{
		using V = tim::Result<ThrowsAssignT, int>;
		V v(100);
		try {
			v = 42;
			REQUIRE(false);
		} catch (...) { /* ... */
		}
		REQUIRE(v.has_value() == true);
		REQUIRE(v.value().value == 100);
	}
	{
		using V = tim::Result<std::string, ThrowsAssignT>;
		V v(tim::Error(100));
		try {
			v = tim::Error(42);
			REQUIRE(false);
		} catch (...) { /* ... */
		}
		REQUIRE(v.has_value() == false);
		REQUIRE(v.error().value == 100);
	}
}

} /* namespace T_assignment */

namespace conv_assignment {

TEST_CASE("Conv assignment") {
	static_assert(std::is_assignable<tim::Result<int, int>, int>::value, "");
	static_assert(!std::is_assignable<tim::Result<int, int>, void*>::value, "");
	static_assert(std::is_assignable<tim::Result<long, long long>, int>::value, "");
	static_assert(std::is_assignable<tim::Result<char, int>, int>::value, "");
	
	static_assert(!std::is_assignable<tim::Result<int, bool>, decltype("meow")>::value, "");

	struct ExplicitBool {
	
		explicit constexpr operator bool () const noexcept { return true; }
	};
	
	static_assert(std::is_assignable<tim::Result<bool, int>, std::true_type>::value, "");
	static_assert(!std::is_assignable<tim::Result<bool, int>, ExplicitBool>::value, "");
	static_assert(!std::is_assignable<tim::Result<bool, int>, std::unique_ptr<char> >::value, "");
	static_assert(!std::is_assignable<tim::Result<bool, int>, decltype(nullptr)>::value, "");
}

} /* namespace conv_assignment */



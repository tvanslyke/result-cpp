#include "catch.hpp"
#include "support/test_convertible.h"
#include "tim/result/Result.hpp"

#include <vector>
#include <type_traits>
#include <string>


template <class T, class E>
struct result_is_instantiatable_with {

	template <class U>
	struct Tag{};

	template <class U, class G, std::size_t = sizeof(tim::Result<U, G>)>
	static constexpr std::true_type test(Tag<U>, Tag<G>, int) { return std::true_type{}; }

	template <class U, class G>
	static constexpr std::false_type test(Tag<U>, Tag<G>, ...) { return std::false_type{}; }

	static constexpr bool value = decltype(test(Tag<T>{}, Tag<E>{}, 0))::value;
};

template <class T, class E>
inline constexpr bool result_is_instantiatable_with_v
	= result_is_instantiatable_with<T, E>::value;

struct takes_init_and_variadic {
	std::vector<int> v;
	std::tuple<int, int> t;
	template <class... Args>
	takes_init_and_variadic(std::initializer_list<int> l, Args &&... args)
		: v(l), t(std::forward<Args>(args)...) {}
};

template <class T>
struct ConstructibleFrom {
	ConstructibleFrom(const T& v): value(v) {}
	ConstructibleFrom(T&& v): value(std::move(v)) {}

	T value;
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
				tim::Result<int,int> e = tim::Error(0);
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

}

namespace default_tests {

struct NonDefaultConstructible {
	constexpr NonDefaultConstructible(int) {}
};

struct NotNoexcept {
	NotNoexcept() noexcept(false) {}
};

struct DefaultCtorThrows {
	DefaultCtorThrows() { throw 42; }
};

void test_default_ctor_sfinae() {
	{
		using V = tim::Result<int, int>;
		static_assert(std::is_default_constructible<V>::value, "");
	}
	{
		using V = tim::Result<NonDefaultConstructible, int>;
		static_assert(!std::is_default_constructible<V>::value, "");
	}
}

void test_default_ctor_noexcept() {
	{
		using V = tim::Result<int, NotNoexcept>;
		static_assert(std::is_nothrow_default_constructible<V>::value, "");
	}
	{
		using V = tim::Result<NotNoexcept, int>;
		static_assert(!std::is_nothrow_default_constructible<V>::value, "");
	}
}

void test_default_ctor_throws() {
	using V = tim::Result<DefaultCtorThrows, int>;
	try {
		V v;
		REQUIRE(false);
	} catch (const int &ex) {
		REQUIRE(ex == 42);
	} catch (...) {
		REQUIRE(false);
	}
}

void test_default_ctor_basic() {
	{
		tim::Result<int, int> v;
		REQUIRE(v.has_value());
		REQUIRE(v.value() == 0);
	}
	{
		tim::Result<int, long> v;
		REQUIRE(v.has_value());
		REQUIRE(v.value() == 0);
	}
	{
		tim::Result<int, NonDefaultConstructible> v;
		REQUIRE(v.has_value());
		REQUIRE(v.value() == 0);
	}
	{
		using V = tim::Result<int, long>;
		constexpr V v;
		static_assert(v.has_value(), "");
		static_assert(v.value() == 0, "");
	}
	{
		using V = tim::Result<int, long>;
		constexpr V v;
		static_assert(v.has_value(), "");
		static_assert(v.value() == 0, "");
	}
	{
		using V = tim::Result<int, NonDefaultConstructible>;
		constexpr V v;
		static_assert(v.has_value(), "");
		static_assert(v.value() == 0, "");
	}
}

TEST_CASE("Default Constructors", "[constructors]") {
	test_default_ctor_sfinae();
	test_default_ctor_noexcept();
	test_default_ctor_throws();
	test_default_ctor_basic();
}

} /* namespace default_tests */

namespace copy_tests {

struct NonT {
	NonT(int v) : value(v) {}
	NonT(const NonT &o) : value(o.value) {}
	int value;
};
static_assert(!std::is_trivially_copy_constructible<NonT>::value, "");

struct NoCopy {
	NoCopy(const NoCopy &) = delete;
};

struct MoveOnly {
	MoveOnly(const MoveOnly &) = delete;
	MoveOnly(MoveOnly &&) = default;
};

struct MoveOnlyNT {
	MoveOnlyNT(const MoveOnlyNT &) = delete;
	MoveOnlyNT(MoveOnlyNT &&) {}
};

struct NTCopy {
	constexpr NTCopy(int v) : value(v) {}
	NTCopy(const NTCopy &that) : value(that.value) {}
	NTCopy(NTCopy &&) = delete;
	int value;
};

static_assert(!std::is_trivially_copy_constructible<NTCopy>::value, "");
static_assert(std::is_copy_constructible<NTCopy>::value, "");

struct TCopy {
	constexpr TCopy(int v) : value(v) {}
	TCopy(TCopy const &) = default;
	TCopy(TCopy &&) = delete;
	int value;
};

static_assert(std::is_trivially_copy_constructible<TCopy>::value, "");

struct TCopyNTMove {
	constexpr TCopyNTMove(int v) : value(v) {}
	TCopyNTMove(const TCopyNTMove&) = default;
	TCopyNTMove(TCopyNTMove&& that) : value(that.value) { that.value = -1; }
	int value;
};

static_assert(std::is_trivially_copy_constructible<TCopyNTMove>::value, "");

void test_copy_ctor_sfinae() {
	{
		using V = tim::Result<int, long>;
		static_assert(std::is_copy_constructible<V>::value, "");
	}
	{
		using V = tim::Result<int, NoCopy>;
		static_assert(!std::is_copy_constructible<V>::value, "");
	}
	{
		using V = tim::Result<int, MoveOnly>;
		static_assert(!std::is_copy_constructible<V>::value, "");
	}
	{
		using V = tim::Result<int, MoveOnlyNT>;
		static_assert(!std::is_copy_constructible<V>::value, "");
	}

	// Make sure we properly propagate triviality
	{
		using V = tim::Result<int, long>;
		static_assert(std::is_trivially_copy_constructible<V>::value, "");
	}
	{
		using V = tim::Result<int, NTCopy>;
		static_assert(!std::is_trivially_copy_constructible<V>::value, "");
		static_assert(std::is_copy_constructible<V>::value, "");
	}
	{
		using V = tim::Result<int, TCopy>;
		static_assert(std::is_trivially_copy_constructible<V>::value, "");
	}
	{
		using V = tim::Result<int, TCopyNTMove>;
		static_assert(std::is_trivially_copy_constructible<V>::value, "");
	}
}

void test_copy_ctor_basic() {
	{
		tim::Result<int, int> v(tim::in_place, 42);
		tim::Result<int, int> v2 = v;
		REQUIRE(v2.has_value());
		REQUIRE(v2.value() == 42);
	}
	{
		tim::Result<int, long> v(tim::in_place_error, 42);
		tim::Result<int, long> v2 = v;
		REQUIRE((!v2.has_value()));
		REQUIRE(v2.error() == 42);
	}
	{
		tim::Result<NonT, int> v(tim::in_place, 42);
		REQUIRE(v.has_value());
		tim::Result<NonT, int> v2(v);
		REQUIRE(v2.has_value());
		REQUIRE(v2.value().value == 42);
	}
	{
		tim::Result<int, NonT> v(tim::in_place_error, 42);
		REQUIRE((!v.has_value()));
		tim::Result<int, NonT> v2(v);
		REQUIRE((!v2.has_value()));
		REQUIRE(v2.error().value == 42);
	}

	{
		constexpr tim::Result<int, int> v(tim::in_place, 42);
		static_assert(v.has_value(), "");
		constexpr tim::Result<int, int> v2 = v;
		static_assert(v2.has_value(), "");
		static_assert(v2.value() == 42, "");
	}
	{
		constexpr tim::Result<int, long> v(tim::in_place_error, 42);
		static_assert((!v.has_value()), "");
		constexpr tim::Result<int, long> v2 = v;
		static_assert((!v2.has_value()), "");
		static_assert(v2.error() == 42, "");
	}
	{
		constexpr tim::Result<TCopy, int> v(tim::in_place, 42);
		static_assert(v.has_value(), "");
		constexpr tim::Result<TCopy, int> v2(v);
		static_assert(v2.has_value(), "");
		static_assert(v2.value().value == 42, "");
	}
	{
		constexpr tim::Result<int, TCopy> v(tim::in_place_error, 42);
		static_assert((!v.has_value()), "");
		constexpr tim::Result<int, TCopy> v2(v);
		static_assert((!v2.has_value()), "");
		static_assert(v2.error().value == 42, "");
	}
	{
		constexpr tim::Result<TCopyNTMove, int> v(tim::in_place, 42);
		static_assert(v.has_value(), "");
		constexpr tim::Result<TCopyNTMove, int> v2(v);
		static_assert(v2.has_value(), "");
		static_assert(v2.value().value == 42, "");
	}
	{
		constexpr tim::Result<int, TCopyNTMove> v(tim::in_place_error, 42);
		static_assert((!v.has_value()), "");
		constexpr tim::Result<int, TCopyNTMove> v2(v);
		static_assert((!v2.has_value()), "");
		static_assert(v2.error().value == 42, "");
	}
}


template <bool HasValue>
constexpr bool test_constexpr_copy_ctor_imp(tim::Result<long, void*> const& v) {
	auto v2 = v;
	return v2.has_value() == v.has_value() &&
		v2.has_value() == HasValue &&
		(HasValue ? v2.value() == v.value() : v2.error() == v.error());
}

void test_constexpr_copy_ctor() {
	// Make sure we properly propagate triviality, which implies constexpr-ness
	using V = tim::Result<long, void*>;
	static_assert(std::is_trivially_destructible<V>::value, "");
	static_assert(std::is_trivially_copy_constructible<V>::value, "");
	static_assert(std::is_trivially_move_constructible<V>::value, "");
	static_assert(!std::is_copy_assignable<tim::Result<const int, void*>>::value, "");
	static_assert(!std::is_move_assignable<tim::Result<const int, void*>>::value, "");
	static_assert(std::is_trivially_copyable<V>::value, "");
	static_assert(test_constexpr_copy_ctor_imp<true>(V(42l)), "");
	static_assert(test_constexpr_copy_ctor_imp<false>(V(tim::Error(nullptr))), "");
	static_assert(test_constexpr_copy_ctor_imp<false>(V(tim::in_place_error, nullptr)), "");
}

TEST_CASE("Copy Constructors", "[constructors]") {
	test_copy_ctor_basic();
	test_copy_ctor_sfinae();
	test_constexpr_copy_ctor();
}

} /* namespace copy_tests */

namespace move_tests {

struct ThrowsMove {
	ThrowsMove(ThrowsMove &&) noexcept(false) {}
};

struct NoCopy {
	NoCopy(const NoCopy &) = delete;
};

struct MoveOnly {
	int value;
	MoveOnly(int v) : value(v) {}
	MoveOnly(const MoveOnly &) = delete;
	MoveOnly(MoveOnly &&) = default;
};

struct MoveOnlyNT {
	int value;
	MoveOnlyNT(int v) : value(v) {}
	MoveOnlyNT(const MoveOnlyNT &) = delete;
	MoveOnlyNT(MoveOnlyNT &&other) : value(other.value) { other.value = -1; }
};

struct NTMove {
	constexpr NTMove(int v) : value(v) {}
	NTMove(const NTMove &) = delete;
	NTMove(NTMove &&that) : value(that.value) { that.value = -1; }
	int value;
};

static_assert(!std::is_trivially_move_constructible<NTMove>::value, "");
static_assert(std::is_move_constructible<NTMove>::value, "");

struct TMove {
	constexpr TMove(int v) : value(v) {}
	TMove(const TMove &) = delete;
	TMove(TMove &&) = default;
	int value;
};

static_assert(std::is_trivially_move_constructible<TMove>::value, "");

struct TMoveNTCopy {
	constexpr TMoveNTCopy(int v) : value(v) {}
	TMoveNTCopy(const TMoveNTCopy& that) : value(that.value) {}
	TMoveNTCopy(TMoveNTCopy&&) = default;
	int value;
};

static_assert(std::is_trivially_move_constructible<TMoveNTCopy>::value, "");


void test_move_noexcept() {
	{
		using V = tim::Result<int, long>;
		static_assert(std::is_nothrow_move_constructible<V>::value, "");
	}
	{
		using V = tim::Result<int, MoveOnly>;
		static_assert(std::is_nothrow_move_constructible<V>::value, "");
	}
	{
		using V = tim::Result<int, MoveOnlyNT>;
		static_assert(!std::is_nothrow_move_constructible<V>::value, "");
	}
	{
		using V = tim::Result<int, ThrowsMove>;
		static_assert(!std::is_nothrow_move_constructible<V>::value, "");
	}
}

void test_move_ctor_sfinae() {
	{
		using V = tim::Result<int, long>;
		static_assert(std::is_move_constructible<V>::value, "");
	}
	{
		using V = tim::Result<int, MoveOnly>;
		static_assert(std::is_move_constructible<V>::value, "");
	}
	{
		using V = tim::Result<int, MoveOnlyNT>;
		static_assert(std::is_move_constructible<V>::value, "");
	}
	{
		using V = tim::Result<int, NoCopy>;
		static_assert(!std::is_move_constructible<V>::value, "");
	}

	{
		using V = tim::Result<int, long>;
		static_assert(std::is_trivially_move_constructible<V>::value, "");
	}
	{
		using V = tim::Result<int, NTMove>;
		static_assert(!std::is_trivially_move_constructible<V>::value, "");
		static_assert(std::is_move_constructible<V>::value, "");
	}
	{
		using V = tim::Result<int, TMove>;
		static_assert(std::is_trivially_move_constructible<V>::value, "");
	}
	{
		using V = tim::Result<int, TMoveNTCopy>;
		static_assert(std::is_trivially_move_constructible<V>::value, "");
	}
}

template <typename T>
struct ResultT { size_t index; T value; };

void test_move_ctor_basic() {
	{
		tim::Result<int, double> v(tim::in_place, 42);
		tim::Result<int, double> v2 = std::move(v);
		REQUIRE(v2.has_value());
		REQUIRE(v2.value() == 42);
	}
	{
		tim::Result<int, long> v(tim::in_place_error, 42);
		tim::Result<int, long> v2 = std::move(v);
		REQUIRE(!v2.has_value());
		REQUIRE(v2.error() == 42);
	}
	{
		tim::Result<MoveOnly, int> v(tim::in_place, 42);
		REQUIRE(v.has_value());
		tim::Result<MoveOnly, int> v2(std::move(v));
		REQUIRE(v2.has_value());
		REQUIRE(v2.value().value == 42);
	}
	{
		tim::Result<int, MoveOnly> v(tim::in_place_error, 42);
		REQUIRE(!v.has_value());
		tim::Result<int, MoveOnly> v2(std::move(v));
		REQUIRE(!v2.has_value());
		REQUIRE(v2.error().value == 42);
	}
	{
		tim::Result<MoveOnlyNT, int> v(tim::in_place, 42);
		REQUIRE(v.has_value());
		tim::Result<MoveOnlyNT, int> v2(std::move(v));
		REQUIRE(v2.has_value());
		REQUIRE(v.value().value == -1);
		REQUIRE(v2.value().value == 42);
	}
	{
		tim::Result<int, MoveOnlyNT> v(tim::in_place_error, 42);
		REQUIRE(!v.has_value());
		tim::Result<int, MoveOnlyNT> v2(std::move(v));
		REQUIRE(!v2.has_value());
		REQUIRE(v.error().value == -1);
		REQUIRE(v2.error().value == 42);
	}

	// Make sure we properly propagate triviality, which implies constexpr-ness (see P0602R4).
	{
		struct {
			constexpr ResultT<int> operator()() const {
				tim::Result<int, int> v(tim::in_place, 42);
				tim::Result<int, int> v2 = std::move(v);
				return {v2.has_value() ? 0u : 1u, std::move(v2).value()};
			}
		} test;
		constexpr auto result = test();
		static_assert(result.index == 0, "");
		static_assert(result.value == 42, "");
	}
	{
		struct {
			constexpr ResultT<long> operator()() const {
				tim::Result<int, long> v(tim::in_place_error, 42);
				tim::Result<int, long> v2 = std::move(v);
				return {v2.has_value() ? 0u : 1u, std::move(v2).error()};
			}
		} test;
		constexpr auto result = test();
		static_assert(result.index == 1, "");
		static_assert(result.value == 42, "");
	}
	{
		struct {
			constexpr ResultT<TMove> operator()() const {
				tim::Result<TMove, int> v(tim::in_place, 42);
				tim::Result<TMove, int> v2(std::move(v));
				return {v2.has_value() ? 0u : 1u, std::move(v2).value()};
			}
		} test;
		constexpr auto result = test();
		static_assert(result.index == 0, "");
		static_assert(result.value.value == 42, "");
	}
	{
		struct {
			constexpr ResultT<TMove> operator()() const {
				tim::Result<int, TMove> v(tim::in_place_error, 42);
				tim::Result<int, TMove> v2(std::move(v));
				return {v2.has_value() ? 0u : 1u, std::move(v2).error()};
			}
		} test;
		constexpr auto result = test();
		static_assert(result.index == 1, "");
		static_assert(result.value.value == 42, "");
	}
	{
		struct {
			constexpr ResultT<TMoveNTCopy> operator()() const {
				tim::Result<TMoveNTCopy, int> v(tim::in_place, 42);
				tim::Result<TMoveNTCopy, int> v2(std::move(v));
				return {v2.has_value() ? 0u : 1u, std::move(v2).value()};
			}
		} test;
		constexpr auto result = test();
		static_assert(result.index == 0, "");
		static_assert(result.value.value == 42, "");
	}
	{
		struct {
			constexpr ResultT<TMoveNTCopy> operator()() const {
				tim::Result<int, TMoveNTCopy> v(tim::in_place_error, 42);
				tim::Result<int, TMoveNTCopy> v2(std::move(v));
				return {v2.has_value() ? 0u : 1u, std::move(v2).error()};
			}
		} test;
		constexpr auto result = test();
		static_assert(result.index == 1, "");
		static_assert(result.value.value == 42, "");
	}
}


template <bool HasValue>
constexpr bool test_constexpr_ctor_imp(tim::Result<long, void*> const& v) {
	auto copy = v;
	auto v2 = std::move(copy);
	return v2.has_value() == v.has_value()
		&& v2.has_value() == HasValue
		&& (HasValue ? v2.value() == v.value() : v2.error() == v.error());
}

void test_constexpr_move_ctor() {
	using V = tim::Result<long, void*>;
	static_assert(std::is_trivially_destructible<V>::value, "");
	static_assert(std::is_trivially_copy_constructible<V>::value, "");
	static_assert(std::is_trivially_move_constructible<V>::value, "");
	static_assert(std::is_copy_assignable<V>::value, "");
	static_assert(std::is_move_assignable<V>::value, "");
	static_assert(std::is_trivially_move_constructible<V>::value, "");
	static_assert(test_constexpr_ctor_imp<true>(V(42l)), "");
	static_assert(test_constexpr_ctor_imp<false>(V(tim::Error(nullptr))), "");
	static_assert(test_constexpr_ctor_imp<false>(V(tim::in_place_error, nullptr)), "");
}

TEST_CASE("Move Constructors", "[constructors]") {
	test_move_ctor_basic();
	test_move_noexcept();
	test_move_ctor_sfinae();
	test_constexpr_move_ctor();
}

} /* namespace move_tests */

namespace in_place_tests {

void test_ctor_sfinae() {
	{
		using V = tim::Result<int, int>;
		static_assert(std::is_constructible<V, tim::in_place_t, int>::value, "");
		static_assert(!test_convertible<V, tim::in_place_t, int>(), "");
	}
	{
		using V = tim::Result<int, long>;
		static_assert(std::is_constructible<V, tim::in_place_error_t, int>::value, "");
		static_assert(!test_convertible<V, tim::in_place_error_t, int>(), "");
	}
	{
		using V = tim::Result<int, int *>;
		static_assert(std::is_constructible<V, tim::in_place_error_t, int *>::value, "");
		static_assert(!test_convertible<V, tim::in_place_error_t, int *>(), "");
	}
	{
		using V = tim::Result<int, int *>;
		static_assert(!std::is_constructible<V, tim::in_place_t, int *>::value, "");
		static_assert(!test_convertible<V, tim::in_place_t, int *>(), "");
	}
	{
		using V = tim::Result<int, long>;
		static_assert(std::is_constructible<V, tim::in_place_t, int>::value, "");
		static_assert(std::is_constructible<V, tim::in_place_error_t, long>::value, "");
	}
}

void test_ctor_basic() {
	{
		constexpr tim::Result<int, int> v(tim::in_place, 42);
		static_assert(v.has_value(), "");
		static_assert(v.value() == 42, "");
	}
	{
		constexpr tim::Result<const int, int> v(tim::in_place, 42);
		static_assert(v.has_value(), "");
		static_assert(v.value() == 42, "");
	}
	{
		constexpr tim::Result<int, long> v(tim::in_place_error, 42);
		static_assert(!v.has_value(), "");
		static_assert(v.error() == 42, "");
	}
	{
		constexpr tim::Result<const int, int> v(tim::in_place_error, 42);
		static_assert(!v.has_value(), "");
		static_assert(v.error() == 42, "");
	}
	{
		using V = tim::Result<const int, int>;
		int x = 42;
		V v(tim::in_place, x);
		REQUIRE(v.has_value());
		REQUIRE(v.value() == x);
	}
	{
		using V = tim::Result<const int, int>;
		int x = 42;
		V v(tim::in_place_error, x);
		REQUIRE(!v.has_value());
		REQUIRE(v.error() == x);
	}
}

TEST_CASE("in_place Constructors", "[constructors]") {
	test_ctor_basic();
	test_ctor_sfinae();
}

} /* namespace in_place_tests */

namespace in_place_init_list_tests {

struct InitList {
	std::size_t size;
	constexpr InitList(std::initializer_list<int> il) : size(il.size()) {}
};

struct InitListArg {
	std::size_t size;
	int value;
	constexpr InitListArg(std::initializer_list<int> il, int v)
			: size(il.size()), value(v) {}
};

void test_ctor_sfinae() {
	using IL = std::initializer_list<int>;
	{ // just init list
		using V = tim::Result<InitList, InitListArg>;
		static_assert(std::is_constructible<V, tim::in_place_t, IL>::value, "");
		static_assert(!test_convertible<V, tim::in_place_t, IL>(), "");
	}
	{ // too many arguments
		using V = tim::Result<InitList, InitListArg>;
		static_assert(!std::is_constructible<V, tim::in_place_t, IL, int>::value, "");
		static_assert(!test_convertible<V, tim::in_place_t, IL, int>(), "");
	}
	{ // too few arguments
		using V = tim::Result<InitList, InitListArg>;
		static_assert(!std::is_constructible<V, tim::in_place_error_t, IL>::value, "");
		static_assert(!test_convertible<V, tim::in_place_error_t, IL>(), "");
	}
	{ // init list and arguments
		using V = tim::Result<InitList, InitListArg>;
		static_assert(std::is_constructible<V, tim::in_place_error_t, IL, int>::value, "");
		static_assert(!test_convertible<V, tim::in_place_error_t, IL, int>(), "");
	}
	{ // not constructible from arguments
		using V = tim::Result<InitList, int>;
		static_assert(!std::is_constructible<V, tim::in_place_error_t, IL>::value, "");
		static_assert(!test_convertible<V, tim::in_place_error_t, IL>(), "");
	}
}

void test_ctor_basic() {
	{
		constexpr tim::Result<InitList, InitListArg> v(tim::in_place, {1, 2, 3});
		static_assert(v.has_value());
		static_assert(v->size == 3);
	}
	{
		constexpr tim::Result<InitList, InitList> v(tim::in_place_error, {1, 2, 3});
		static_assert(!v.has_value(), "");
		static_assert(v.error().size == 3, "");
	}
	{
		constexpr tim::Result<InitList, InitListArg> v(tim::in_place_error, {1, 2, 3, 4}, 42);
		static_assert(!v.has_value(), "");
		static_assert(v.error().size == 4, "");
		static_assert(v.error().value == 42, "");
	}
}

TEST_CASE("In-Place Initializer List Constructors") {
	test_ctor_basic();
	test_ctor_sfinae();
}

} /* namespace in_place_init_list_tests */


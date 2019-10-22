#include "catch.hpp"
#include "tim/result/Result.hpp"
#include <memory>
#include <vector>
#include <tuple>
#include "support/archetypes.h"
#include "support/test_convertible.h"
#include "support/test_macros.h"
#include "support/variant_test_helpers.h"


namespace {
struct takes_init_and_variadic {
	std::vector<int> v;
	std::tuple<int, int> t;
	template <class... Args>
	takes_init_and_variadic(std::initializer_list<int> l, Args &&... args)
			: v(l), t(std::forward<Args>(args)...) {}
};
}

TEST_CASE("Emplace", "[emplace]") {
		{
				tim::Result<std::unique_ptr<int>,int> e;
				e.emplace(new int{42});
				REQUIRE(e);
				REQUIRE(**e == 42);
		}

		{
				tim::Result<std::vector<int>,int> e;
				e.emplace({0,1});
				REQUIRE(e);
				REQUIRE((*e)[0] == 0);
				REQUIRE((*e)[1] == 1);
		}

		{
				tim::Result<std::tuple<int,int>,int> e;
				e.emplace(2,3);
				REQUIRE(e);
				REQUIRE(std::get<0>(*e) == 2);
				REQUIRE(std::get<1>(*e) == 3);
		}

		{
				tim::Result<takes_init_and_variadic,int> e = tim::make_error(0);
				e.emplace({0,1}, 2, 3);
				REQUIRE(e);
				REQUIRE(e->v[0] == 0);
				REQUIRE(e->v[1] == 1);
				REQUIRE(std::get<0>(e->t) == 2);
				REQUIRE(std::get<1>(e->t) == 3);
		}
}


namespace emplacement {

template <class Var, class... Args>
constexpr auto test_emplace_exists_imp(int) -> decltype(
		std::declval<Var>().emplace(std::declval<Args>()...), true) {
	return true;
}

template <class, class...>
constexpr auto test_emplace_exists_imp(long) -> bool {
	return false;
}

template <class Var, class... Args> constexpr bool emplace_exists() {
	return test_emplace_exists_imp<Var, Args...>(0);
}

struct OnlyOneCtor {
	OnlyOneCtor() = delete;
	OnlyOneCtor(const OnlyOneCtor&) = delete;
	OnlyOneCtor(OnlyOneCtor&&) = delete;

	OnlyOneCtor(int v, const char* s) noexcept: int_val(v), str_val(s) {

	}

	int int_val;
	const char* str_val;
};

struct OnlyOneNoexceptCtor {
	OnlyOneNoexceptCtor() = delete;
	OnlyOneNoexceptCtor(const OnlyOneNoexceptCtor&) = delete;
	OnlyOneNoexceptCtor(OnlyOneNoexceptCtor&&) = delete;

	OnlyOneNoexceptCtor(double): int_val(-1), str_val("double") {

	}

	OnlyOneNoexceptCtor(int v, const char* s) noexcept: int_val(v), str_val(s) {

	}

	int int_val;
	const char* str_val;
};


void test_emplace_sfinae() {
	{
		using V = tim::Result<int, void *>;
		using V2 = tim::Result<void *, int>;
		using V3 = tim::Result<TestTypes::NoCtors, int>; 
		using V4 = tim::Result<const void*, int>; 
		using V5 = tim::Result<OnlyOneCtor, int>; 
		using V6 = tim::Result<OnlyOneNoexceptCtor, int>; 
		static_assert(emplace_exists<V>(), "");
		static_assert(emplace_exists<V, int>(), "");
		static_assert(!emplace_exists<V, decltype(nullptr)>(), "cannot construct");
		static_assert(emplace_exists<V2, decltype(nullptr)>(), "");
		static_assert(emplace_exists<V2, int *>(), "");
		static_assert(!emplace_exists<V2, const int *>(), "");
		static_assert(emplace_exists<V, int>(), "cannot construct");
		static_assert(emplace_exists<V4, const int *>(), "");
		static_assert(emplace_exists<V4, int *>(), "");
		static_assert(!emplace_exists<V3>(), "cannot construct");
		static_assert(!emplace_exists<V3, TestTypes::NoCtors>(), "cannot construct");

		static_assert(!emplace_exists<V5, OnlyOneCtor>(), "cannot construct");
		static_assert(!emplace_exists<V5, const OnlyOneCtor&>(), "cannot construct");
		static_assert(!emplace_exists<V5, OnlyOneCtor&&>(), "cannot construct");
		static_assert(!emplace_exists<V5>(), "cannot construct");
		static_assert(!emplace_exists<V5, int>(), "cannot construct");
		static_assert(emplace_exists<V5, int, const char*>(), "cannot construct");

		static_assert(!emplace_exists<V6, OnlyOneNoexceptCtor>(), "cannot construct");
		static_assert(!emplace_exists<V6, const OnlyOneNoexceptCtor&>(), "cannot construct");
		static_assert(!emplace_exists<V6, OnlyOneNoexceptCtor&&>(), "cannot construct");
		static_assert(!emplace_exists<V6>(), "cannot construct");
		static_assert(!emplace_exists<V6, int>(), "cannot construct");
		static_assert(!emplace_exists<V6, double>(), "cannot construct");
		static_assert(emplace_exists<V6, int, const char*>(), "cannot construct");
	}
}

void test_basic() {
	{
		using V = tim::Result<int, int>;
		V v(42);
		auto& ref1 = v.emplace();
		static_assert(std::is_same_v<int&, decltype(ref1)>, "");
		REQUIRE(v.value() == 0);
		REQUIRE(&ref1 == &v.value());
		auto& ref2 = v.emplace(42);
		static_assert(std::is_same_v<int&, decltype(ref2)>, "");
		REQUIRE(v.value() == 42);
		REQUIRE(&ref2 == &v.value());
	}
	{
		using V = tim::Result<int, long>;
		const int x = 100;
		V v(tim::in_place, -1);
		// default emplace a value
		auto& ref1 = v.emplace();
		static_assert(std::is_same_v<int&, decltype(ref1)>, "");
		REQUIRE(v.value() == 0);
		REQUIRE(&ref1 == &v.value());
		using V2 = tim::Result<OnlyOneCtor, long>;
		V2 v2(tim::in_place, 1, "asdf");
		const char* s = "zxvc";
		auto& ref2 = v2.emplace(3, s);
		static_assert(std::is_same_v<OnlyOneCtor&, decltype(ref2)>, "");
		REQUIRE(v2.value().int_val == 3);
		REQUIRE(v2.value().str_val == s);
	}
}

} /* namespace emplacement */

TEST_CASE("Emplacement", "[emplace]") {
	emplacement::test_basic();
	emplacement::test_emplace_sfinae();
}

namespace emplacement_init_list {

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

template <class Var, class... Args>
constexpr auto test_emplace_exists_imp(int) -> decltype(
		std::declval<Var>().template emplace(std::declval<Args>()...), true) {
	return true;
}

template <class, class...>
constexpr auto test_emplace_exists_imp(long) -> bool {
	return false;
}

template <class Var, class... Args> constexpr bool emplace_exists() {
	return test_emplace_exists_imp<Var, Args...>(0);
}

void test_emplace_sfinae() {
	using V1 = tim::Result<TestTypes::NoCtors, int>;
	using V2 = tim::Result<InitList, int>;
	using V3 = tim::Result<InitListArg, int>;
	
	using IL = std::initializer_list<int>;
	static_assert(!emplace_exists<V1, IL>(), "no such constructor");
	static_assert(emplace_exists<V2, IL>(), "");
	static_assert(!emplace_exists<V2, int>(), "args don't match");
	static_assert(!emplace_exists<V2, IL, int>(), "too many args");
	static_assert(emplace_exists<V3, IL, int>(), "");
	static_assert(!emplace_exists<V3, int>(), "args don't match");
	static_assert(!emplace_exists<V3, IL>(), "too few args");
	static_assert(!emplace_exists<V3, IL, int, int>(), "too many args");
}

void test_basic() {
	using V1 = tim::Result<InitList, int>;
	using V2 = tim::Result<InitListArg, int>;
	V1 v1(tim::in_place_error, -1);
	V2 v2(tim::in_place_error, 0);
	auto& ref1 = v1.emplace({1, 2, 3});
	static_assert(std::is_same_v<InitList&, decltype(ref1)>, "");
	REQUIRE(v1.value().size == 3);
	REQUIRE(&ref1 == &v1.value());
	auto& ref2 = v2.emplace({1, 2, 3, 4}, 42);
	static_assert(std::is_same_v<InitListArg&, decltype(ref2)>, "");
	REQUIRE(v2.value().size == 4);
	REQUIRE(v2.value().value == 42);
	REQUIRE(&ref2 == &v2.value());
	auto& ref3 = v1.emplace({1});
	static_assert(std::is_same_v<InitList&, decltype(ref3)>, "");
	REQUIRE(v1.value().size == 1);
	REQUIRE(&ref3 == &v1.value());
}

} /* namespace emplacement_init_list */

TEST_CASE("Emplacement Init List", "[emplace]") {
	emplacement_init_list::test_basic();
	emplacement_init_list::test_emplace_sfinae();
}

#include "catch.hpp"
#include "tim/result/Result.hpp"

#include <string>

using std::string;

tim::Result<int, string> getInt3(int val) { return val; }

tim::Result<int, string> getInt2(int val) { return val; }

tim::Result<int, int> operation1() { return 42; }

tim::Result<std::string, int> operation2(int const val) { return "Bananas"; }

struct a {};
struct b : a {};

auto doit() -> tim::Result<std::unique_ptr<b>, int> {
    return tim::make_error(0);
}

TEST_CASE("Issue 23", "[issues.23]") {
    tim::Result<std::unique_ptr<a>, int> msg = doit();
    REQUIRE(!msg.has_value());    
}

TEST_CASE("Issue 26", "[issues.26]") {
  tim::Result<a, int> exp = tim::Result<b, int>(tim::in_place_error, 0);
  REQUIRE(!exp.has_value());
}

struct foo {
  foo() = default;
  foo(foo &) = delete;
  foo(foo &&){};
};

TEST_CASE("Issue 29", "[issues.29]") {
  std::vector<foo> v;
  v.emplace_back();
  tim::Result<std::vector<foo>, int> ov = std::move(v);
  REQUIRE(ov->size() == 1);
}

tim::Result<int, std::string> error() {
  return tim::make_error(std::string("error1 "));
}

struct i31{
  int i;
};
TEST_CASE("Issue 31", "[issues.31]") {
    const tim::Result<i31, int> a = i31{42};
    a->i;

    tim::Result< void, std::string > result;
    tim::Result< void, std::string > result2 = result;
    result2 = result;
}

tim::Result<void, std::string> voidWork() { return {}; }
tim::Result<int, std::string> work2() { return 42; }
void errorhandling(std::string){}

struct non_copyable {
	non_copyable(non_copyable&&) = default;
	non_copyable(non_copyable const&) = delete;
	non_copyable() = default;
};

TEST_CASE("Issue 43", "[issues.43]") {
	auto result = tim::Result<void, std::string>{};
	result = tim::make_error(std::string{ "foo" });
}

#if !(__GNUC__ <= 5)
#include <memory>

using MaybeDataPtr = tim::Result<int, std::unique_ptr<int>>;

MaybeDataPtr test(int i) noexcept
{
  return std::move(i);
}

MaybeDataPtr test2(int i) noexcept
{
  return std::move(i);
}

#endif

tim::Result<int, std::unique_ptr<std::string>> func()
{
  return 1;
}

TEST_CASE("Issue 61", "[issues.61]") {
  REQUIRE(func().value() == 1);
}

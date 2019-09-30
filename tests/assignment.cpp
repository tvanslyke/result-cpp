#include "catch.hpp"
#include "tim/result/Result.hpp"

TEST_CASE("Simple assignment", "[assignment.simple]") {
  tim::Result<int, int> e1 = 42;
  tim::Result<int, int> e2 = 17;
  tim::Result<int, int> e3 = 21;
  tim::Result<int, int> e4 = tim::make_error(42);
  tim::Result<int, int> e5 = tim::make_error(17);
  tim::Result<int, int> e6 = tim::make_error(21);

  e1 = e2;
  REQUIRE(e1);
  REQUIRE(*e1 == 17);
  REQUIRE(e2);
  REQUIRE(*e2 == 17);

  e1 = std::move(e2);
  REQUIRE(e1);
  REQUIRE(*e1 == 17);
  REQUIRE(e2);
  REQUIRE(*e2 == 17);

  e1 = 42;
  REQUIRE(e1);
  REQUIRE(*e1 == 42);

  auto unex = tim::make_error(12);
  e1 = unex;
  REQUIRE(!e1);
  REQUIRE(e1.error() == 12);

  e1 = tim::make_error(42);
  REQUIRE(!e1);
  REQUIRE(e1.error() == 42);

  e1 = e3;
  REQUIRE(e1);
  REQUIRE(*e1 == 21);

  e4 = e5;
  REQUIRE(!e4);
  REQUIRE(e4.error() == 17);

  e4 = std::move(e6);
  REQUIRE(!e4);
  REQUIRE(e4.error() == 21);

  e4 = e1;
  REQUIRE(e4);
  REQUIRE(*e4 == 21);
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

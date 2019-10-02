#include "catch.hpp"
#include "tim/result/Result.hpp"

TEST_CASE("Bad Result Access", "[bad_result_access]") {
  static_assert(std::is_base_of<std::exception, std::bad_result_access<>>::value,
                "");
  static_assert(std::is_base_of<std::bad_result_access<>, std::bad_result_access<int>>::value,
                "");
  static_assert(noexcept(std::bad_variant_access{}), "must be noexcept");
  static_assert(noexcept(std::bad_variant_access{}.what()), "must be noexcept");
  std::bad_variant_access ex;
  assert(ex.what());

  return 0;
}


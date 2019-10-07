#include "tim/result/Result.hpp"
#include <type_traits>

struct NotCopyConstructible
{
    NotCopyConstructible() = default;
    NotCopyConstructible(NotCopyConstructible const&) = delete;
};

int main(int, char**)
{
    static_assert(!std::is_copy_constructible_v<NotCopyConstructible>);

    tim::Result<NotCopyConstructible, int> v;
    tim::Result<NotCopyConstructible, int> v1(v);
}

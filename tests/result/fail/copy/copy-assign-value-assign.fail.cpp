#include "tim/result/Result.hpp"
#include <type_traits>

struct NotCopyAssignable
{
    NotCopyAssignable() = default;
    NotCopyAssignable(NotCopyAssignable const&) noexcept {}
    NotCopyAssignable& operator=(NotCopyAssignable const&) {
        return *this;
    }
};

int main(int, char**)
{
    static_assert(std::is_copy_constructible_v<NotCopyConstructible>);
    static_assert(!std::is_copy_constructible_v<NotCopyAssignable>);

    tim::Result<NotCopyAssignable, int> v;
    tim::Result<NotCopyAssignable, int> v2;
    v2 = v;
}

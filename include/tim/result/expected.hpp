#ifndef TIM_RESULT_HPP
#define TIM_RESULT_HPP

#include "tim/result/Result.hpp"

namespace tim {

inline namespace expected {

template <class T, class E>
using expected = tim::result::Result<T, E>;

template <class E>
using unexpected = tim::result::Error<E>;

template <
	class E,
	std::enable_if<
		std::is_constructible_v<
			unexpected<std::decay_t<E>>,
			Err&&
		>,
		bool
	> = false
>
constexpr unexpected<std::decay_t<E>> make_unexpected(E&& err) {
	return unexpected<std::decay_t<E>>(std::forward<E>(err));
}

template <class E>
using bad_expected_access = tim::result::BadExpectedAccess<E>;

template <class E>
using unexpect_t = tim::result::in_place_error;

inline constexpr unexpect_t unexpect = tim::result::in_place_error;

using in_place_t = tim::result::in_place_t;

inline constexpr in_place_t in_place = tim::result::in_place;

using tim::result::operator==;
using tim::result::operator!=;
using tim::result::swap;

namespace traits = tim::result::traits;

} /* inline namespace expected */

} /* namespace tim */

#endif /* TIM_RESULT_HPP */

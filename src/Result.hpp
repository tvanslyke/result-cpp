#ifndef TIM_RESULT_HPP
#define TIM_RESULT_HPP

#include <type_traits>

namespace tim {

inline namespace result {

template <class T, class E>
struct Result;

inline constexpr struct valueless_tag_t {} valueless_tag;

namespace detail {

template <class T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <class T>
using is_cv_void = std::is_same<std::remove_cv_t<T>, void>;

template <class T>
inline constexpr bool is_cv_void = std::is_same_v<std::remove_cv_t<T>, void>;



template <class T>
struct ManualScopeGuard {

	~ManualScopeGuard() {
		if(active) {
			std::invoke(action);
		}
	}

	T action;
	bool active;
};

template <class T>
ManualScopeGuard<std::decay_t<T>> make_manual_scope_guard(T&& action) {
	return ManualScopeGuard<std::decay_t<T>>{std::forward<T>(action), true};
}


struct EmptyAlternative {
	constexpr EmptyAlternative() = default;
	constexpr EmptyAlternative(const EmptyAlternative&) = default;
	constexpr EmptyAlternative(EmptyAlternative&&) = default;
	constexpr EmptyAlternative& operator=(const EmptyAlternative&) = default;
	constexpr EmptyAlternative& operator=(EmptyAlternative&&) = default;
	constexpr EmptyAlternative(valueless_tag_t) {}
};

enum class MemberStatus {
	Defaulted, Deleted, Defined
};

template <MemberStatus S, class T, class E>
struct ResultUnionImpl;

template <class T, class E>
using ResultData = std::conditional_t<
	std::conjunction_v<std::is_destructible<T>, std::is_destructible<E>>,
	std::conditional_t<
		std::conjunction_v<std::is_trivially_destructible<T>, std::is_trivially_destructible<E>>,
		ResultUnionImpl<MemberStatus::Defaulted, T, E>,
		ResultUnionImpl<MemberStatus::Defined, T, E>
	>,
	ResultUnionImpl<MemberStatus::Deleted, T, E>
>;

using value_tag_t = std::in_place_index_t<0>;
using error_tag_t = std::in_place_index_t<1>;
using empty_tag_t = std::in_place_index_t<2>;

inline constexpr value_tag_t value_tag = value_tag_t{};
inline constexpr error_tag_t error_tag = error_tag_t{};
inline constexpr empty_tag_t empty_tag = empty_tag_t{};

template <class T, class E>
struct ResultUnionImpl<MemberStatus::Defaulted, T, E> {

	constexpr ResultUnionImpl() = default;
	constexpr ResultUnionImpl(const ResultUnionImpl&) = default;
	constexpr ResultUnionImpl(ResultUnionImpl&&) = default;
	constexpr ResultUnionImpl& operator=(const ResultUnionImpl&) = default;
	constexpr ResultUnionImpl& operator=(ResultUnionImpl&&) = default;

	constexpr ResultUnionImpl(empty_tag_t): empty() {}

	template <class ... Args>
	constexpr ResultUnionImpl(value_tag_t, Args&& ... args):
		value(std::forward<Args>(args)...)
	{
		
	}

	template <class U, class ... Args>
	constexpr ResultUnionImpl(value_tag_t, std::initializer_list<U> ilist, Args&& ... args):
		value(ilist, std::forward<Args>(args)...)
	{
		
	}

	template <class ... Args>
	constexpr ResultUnionImpl(error_tag_t, Args&& ... args):
		error(std::forward<Args>(args)...)
	{
		
	}

	template <class U, class ... Args>
	constexpr ResultUnionImpl(error_tag_t, std::initializer_list<U> ilist, Args&& ... args):
		error(ilist, std::forward<Args>(args)...)
	{
		
	}

	union {
		EmptyAlternative empty = EmptyAlternative();
		std::conditional_t<is_cv_void_v<T>, EmptyAlternative, T> value;
		E error;
	};
};

template <class T, class E>
struct ResultUnionImpl<MemberStatus::Deleted, T, E> {

	constexpr ResultUnionImpl() = default;
                  
	constexpr ResultUnionImpl(const ResultUnionImpl&) = default;
	constexpr ResultUnionImpl(ResultUnionImpl&&) = default;

	constexpr ResultUnionImpl& operator=(const ResultUnionImpl&) = default;
	constexpr ResultUnionImpl& operator=(ResultUnionImpl&&) = default;

	constexpr ResultUnionImpl(empty_tag_t): empty() {}

	~ResultUnionImpl() = delete;	

	template <class ... Args>
	constexpr ResultUnionImpl(value_tag_t, Args&& ... args):
		value(std::forward<Args>(args)...)
	{
		
	}

	template <class U, class ... Args>
	constexpr ResultUnionImpl(value_tag_t, std::initializer_list<U> ilist, Args&& ... args):
		value(ilist, std::forward<Args>(args)...)
	{
		
	}

	template <class ... Args>
	constexpr ResultUnionImpl(std::in_place_index_t<I>, Args&& ... args):
		error(std::forward<Args>(args)...)
	{
		
	}

	template <class U, class ... Args>
	constexpr ResultUnionImpl(std::in_place_index_t<I>, std::initializer_list<U> ilist, Args&& ... args):
		error(ilist, std::forward<Args>(args)...)
	{
		
	}

	union {
		EmptyAlternative empty = EmptyAlternative();
		std::conditional_t<is_cv_void_v<T>, EmptyAlternative, T> value;
		E error;
	};
};

template <class T, class E>
struct ResultUnionImpl<MemberStatus::Defined, T, E> {

	constexpr ResultUnionImpl() = default;
                     
	constexpr ResultUnionImpl(const ResultUnionImpl&) = default;
	constexpr ResultUnionImpl(ResultUnionImpl&&) = default;

	constexpr ResultUnionImpl& operator=(const ResultUnionImpl&) = default;
	constexpr ResultUnionImpl& operator=(ResultUnionImpl&&) = default;

	constexpr ResultUnionImpl(empty_tag_t): empty() {}

	~ResultUnionImpl() {}

	template <class ... Args>
	constexpr ResultUnionImpl(value_tag_t, Args&& ... args):
		value(std::forward<Args>(args)...)
	{
		
	}

	template <class U, class ... Args>
	constexpr ResultUnionImpl(value_tag_t, std::initializer_list<U> ilist, Args&& ... args):
		value(ilist, std::forward<Args>(args)...)
	{
		
	}

	template <class ... Args>
	constexpr ResultUnionImpl(std::in_place_index_t<I>, Args&& ... args):
		error(std::forward<Args>(args)...)
	{
		
	}

	template <class U, class ... Args>
	constexpr ResultUnionImpl(std::in_place_index_t<I>, std::initializer_list<U> ilist, Args&& ... args):
		error(ilist, std::forward<Args>(args)...)
	{
		
	}

	union {
		EmptyAlternative empty = EmptyAlternative();
		std::conditional_t<is_cv_void_v<T>, EmptyAlternative, T> value;
		E error;
	};
};

template <class T, class E>
struct ResultBaseMethods {

	constexpr ResultBaseMethods() = default;

	template <class ... Args>
	constexpr ResultBaseMethods(bool has_val, Args&& ... args):
		data_(std::forward<Args>(args)...),
		has_value_(has_val)
	{
		
	}

	template <class T, class ... Args>
	constexpr ResultBaseMethods(bool has_val, std::initializer_list<T> ilist, Args&& ... args):
		data_(ilist, std::forward<Args>(args)...),
		has_value_(has_val)
	{
		
	}

	constexpr const T& value() const { return *std::launder(std::addressof(data_.value)); }
	constexpr       T& value()       { return *std::launder(std::addressof(data_.value)); }

	constexpr const E& error() const { return *std::launder(std::addressof(data_.error)); }
	constexpr       E& error()       { return *std::launder(std::addressof(data_.error)); }

	constexpr const bool& has_value() const noexcept { return base_.has_value(); }
	constexpr bool&       has_value()       noexcept { return base_.has_value(); }

	constexpr void destruct_value() noexcept {
		if constexpr(!is_cv_void_v<T> && !std::is_trivially_destructible_v<T>) {
			std::destroy_at(&data_.value);
		}
	}

	constexpr void destruct_error() noexcept {
		if constexpr(!std::is_trivially_destructible_v<E>) {
			std::destroy_at(&data_.error);
		}
	}

	constexpr void destruct() noexcept {
		if(has_value()) {
			destruct_value();
		} else {
			destruct_error();
		}
	}

	ResultUnion<T, E> data_;
	bool has_value_ = true;
};

template <MemberStatus S, class T, class E>
struct ResultDestructor;

template <MemberStatus S, class T, class E>
struct ResultDefaultConstructor;

template <MemberStatus S, class T, class E>
struct ResultCopyConstructor;

template <MemberStatus S, class T, class E>
struct ResultMoveConstructor;

template <MemberStatus S, class T, class E>
struct ResultCopyAssign;

template <MemberStatus S, class T, class E>
struct ResultMoveAssign;

template <class T, class E>
using result_destructor_type = std::conditional_t<
	std::conjunction_v<
		std::disjunction<
			detail::is_cv_void<T>,
			std::is_trivially_destructible<T>
		>,
		std::is_trivially_destructible<E>
	>,
	ResultDestructor<MemberStatus::Defaulted, T, E>,
	ResultDestructor<MemberStatus::Defined, T, E>
>;

template <class T, class E>
using result_default_constructor_type = std::conditional_t<
	std::is_default_constructible_v<T>,
	std::conditional_t<
		std::disjunction<
			detail::is_cv_void<T>,
			std::is_trivially_default_constructible_v<T>
		>,
		ResultDefaultConstructor<MemberStatus::Defaulted, T, E>,
		ResultDefaultConstructor<MemberStatus::Defined, T, E>
	>,
	ResultDefaultConstructor<MemberStatus::Deleted, T, E>
>;


template <class T, class E>
using result_copy_constructor_type = std::conditional_t<
	std::conjunction_v<std::is_copy_constructible<T>, std::is_copy_constructible<E>>,
	std::conditional_t<
		std::conjunction_v<
			std::disjunction<
				detail::is_cv_void<T>,
				std::is_trivially_copy_constructible<T>
			>
			std::is_trivially_copy_constructible<E>
		>,
		ResultCopyConstructor<MemberStatus::Defaulted, T, E>,
		ResultCopyConstructor<MemberStatus::Defined, T, E>
	>,
	ResultCopyConstructor<MemberStatus::Deleted, T, E>
>;

template <class T, class E>
using result_move_constructor_type = std::conditional_t<
	std::conjunction_v<std::is_move_constructible<T>, std::is_move_constructible<E>>,
	std::conditional_t<
		std::conjunction_v<
			std::disjunction<
				detail::is_cv_void<T>,
				std::is_trivially_move_constructible<T>
			>,
			std::is_trivially_move_constructible<E>
		>,
		ResultMoveConstructor<MemberStatus::Defaulted, T, E>,
		ResultMoveConstructor<MemberStatus::Defined, T, E>
	>,
	ResultMoveConstructor<MemberStatus::Deleted, T, E>
>;

template <class T, class E>
using result_copy_assign_type = std::conditional_t<
	std::disjunction_v<
		std::conjunction<
			detail::is_cv_void<T>,
			std::is_copy_assignable<E>,
			std::is_copy_constructible<E>
		>,
		std::conjunction<
			std::negation<detail::is_cv_void<T>>,
			std::is_copy_assignable<T>,
			std::is_copy_assignable<E>,
			std::is_copy_constructible<T>,
			std::is_copy_constructible<E>,
			std::disjuction<
				std::is_nothrow_move_constructible<T>,
				std::is_nothrow_move_constructible<E>
			>
		>
	>,
	std::conditional_t<
		std::conjunction_v<
			std::disjunction<
				detail::is_cv_void<T>,
				std::conjunction<
					std::is_trivially_copy_assignable<T>,
					std::is_trivially_copy_constructible<T>,
					std::is_trivially_destructible<T>
				>
			>,
			std::is_trivially_copy_assignable<E>,
			std::is_trivially_copy_constructible<E>,
			std::is_trivially_destructible<E>
		>,
		ResultCopyAssign<MemberStatus::Defaulted, T, E>,
		ResultCopyAssign<MemberStatus::Defined, T, E>
	>,
	ResultCopyAssign<MemberStatus::Deleted, T, E>
>;

template <class T, class E>
using result_move_assign_type = std::conditional_t<
	std::disjunction_v<
		std::conjunction<
			detail::is_cv_void<T>,
			std::is_move_assignable<E>,
			std::is_move_constructible<E>
		>,
		std::conjunction<
			std::negation<detail::is_cv_void<T>>,
			std::is_move_assignable<T>,
			std::is_move_constructible<T>,
			std::is_nothrow_move_assignable<E>,
			std::is_nothrow_move_constructible<E>,
		>
	>,
	std::conditional_t<
		std::conjunction_v<
			std::disjunction<
				detail::is_cv_void<T>,
				std::conjunction<
					std::is_trivially_move_assignable<T>,
					std::is_trivially_move_constructible<T>,
					std::is_trivially_destructible<T>
				>
			>
			std::is_trivially_move_assignable<E>,
			std::is_trivially_move_constructible<E>,
			std::is_trivially_destructible<E>
		>,
		ResultMoveAssign<MemberStatus::Defaulted, T, E>,
		ResultMoveAssign<MemberStatus::Defined, T, E>
	>,
	ResultMoveAssign<MemberStatus::Deleted, T, E>
>;

template <class T, class E>
using result_data_type = result_move_assign_type<T, E>;

template <class T, class E>
struct ResultDestructor<MemberStatus::Defaulted, Alts ...>:
	ResultBaseMethods<T, E>
{
	using base_type = ResultBaseMethods<T, E>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::error;
	using base_type::destruct;
	using base_type::destruct_value;
	using base_type::destruct_error;

	constexpr ResultDestructor() = default;

	constexpr ResultDestructor(const ResultDestructor&) = default;
	constexpr ResultDestructor(ResultDestructor&&) = default;

	constexpr ResultDestructor& operator=(const ResultDestructor&) = default;
	constexpr ResultDestructor& operator=(ResultDestructor&&) = default;
};

template <class T, class E>
struct ResultDestructor<MemberStatus::Defined, T, E>:
	ResultBaseMethods<T, E>
{
	using base_type = ResultBaseMethods<T, E>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::error;
	using base_type::destruct;
	using base_type::destruct_value;
	using base_type::destruct_error;
	
	constexpr ResultDestructor() = default;
	constexpr ResultDestructor(const ResultDestructor&) = default;
	constexpr ResultDestructor(ResultDestructor&&) = default;
	constexpr ResultDestructor& operator=(const ResultDestructor&) = default;
	constexpr ResultDestructor& operator=(ResultDestructor&&) = default;

	~ResultDestructor() {
		this->destruct();
	}
};

template <class T, class E>
struct ResultDefaultConstructor<MemberStatus::Defaulted, T, E>:
	result_destructor_type<T, E>
{
	using base_type = result_destructor_type<T, E>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::error;
	using base_type::destruct;
	using base_type::destruct_value;
	using base_type::destruct_error;

	constexpr ResultDefaultConstructor() = default;
	constexpr ResultDefaultConstructor(const ResultDefaultConstructor&) = default;
	constexpr ResultDefaultConstructor(ResultDefaultConstructor&&) = default;
	constexpr ResultDefaultConstructor& operator=(const ResultDefaultConstructor&) = default;
	constexpr ResultDefaultConstructor& operator=(ResultDefaultConstructor&&) = default;
};

template <class T, class E>
struct ResultDefaultConstructor<MemberStatus::Deleted, T, E>:
	result_destructor_type<T, E>
{
	using base_type = result_destructor_type<T, E>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::error;
	using base_type::destruct;
	using base_type::destruct_value;
	using base_type::destruct_error;

	constexpr ResultDefaultConstructor() = delete;
	constexpr ResultDefaultConstructor(const ResultDefaultConstructor&) = default;
	constexpr ResultDefaultConstructor(ResultDefaultConstructor&&) = default;
	constexpr ResultDefaultConstructor& operator=(const ResultDefaultConstructor&) = default;
	constexpr ResultDefaultConstructor& operator=(ResultDefaultConstructor&&) = default;
};

template <class T, class E>
struct ResultDefaultConstructor<MemberStatus::Defined, T, E>:
	result_destructor_type<First, T, E>
{
	using base_type = result_destructor_type<T, E>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::error;
	using base_type::destruct;
	using base_type::destruct_value;
	using base_type::destruct_error;
	
	constexpr ResultDefaultConstructor() noexcept(std::is_nothrow_default_constructible_v<First>):
		base_type(0, std::in_place_index<0>)
	{
		
	}

	constexpr ResultDefaultConstructor(const ResultDefaultConstructor& other) = default;
	constexpr ResultDefaultConstructor(ResultDefaultConstructor&&) = default;
	constexpr ResultDefaultConstructor& operator=(const ResultDefaultConstructor&) = default;
	constexpr ResultDefaultConstructor& operator=(ResultDefaultConstructor&&) = default;
};

template <class T, class E>
struct ResultCopyConstructor<MemberStatus::Defaulted, T, E>:
	result_default_constructor_type<T, E>
{
	using base_type = result_default_constructor_type<T, E>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::error;
	using base_type::destruct;
	using base_type::destruct_value;
	using base_type::destruct_error;

	constexpr ResultCopyConstructor() = default;
	constexpr ResultCopyConstructor(const ResultCopyConstructor&) = default;
	constexpr ResultCopyConstructor(ResultCopyConstructor&&) = default;
	constexpr ResultCopyConstructor& operator=(const ResultCopyConstructor&) = default;
	constexpr ResultCopyConstructor& operator=(ResultCopyConstructor&&) = default;
};

template <class T, class E>
struct ResultCopyConstructor<MemberStatus::Deleted, T, E>:
	result_default_constructor_type<T, E>
{
	using base_type = result_default_constructor_type<T, E>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::error;
	using base_type::destruct;
	using base_type::destruct_value;
	using base_type::destruct_error;

	constexpr ResultCopyConstructor() = default;
	constexpr ResultCopyConstructor(const ResultCopyConstructor&) = delete;
	constexpr ResultCopyConstructor(ResultCopyConstructor&&) = default;
	constexpr ResultCopyConstructor& operator=(const ResultCopyConstructor&) = default;
	constexpr ResultCopyConstructor& operator=(ResultCopyConstructor&&) = default;
};

template <class T, class E>
struct ResultCopyConstructor<MemberStatus::Defined, T, E>
{
	using base_type = result_default_constructor_type<T, E>;
	using index_type = typename base_type::index_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::error;
	using base_type::destruct;
	using base_type::destruct_value;
	using base_type::destruct_error;

	
	constexpr ResultCopyConstructor() = default;
	constexpr ResultCopyConstructor(const ResultCopyConstructor& other):
		base_([&other]{
			if(other.has_value()) {
				return base_type(true, value_tag, other.value);
			} else {
				return base_type(false, error_tag, other.error);
			}
		}())
	{
		
	}

	constexpr ResultCopyConstructor(ResultCopyConstructor&&) = default;

	constexpr ResultCopyConstructor& operator=(const ResultCopyConstructor&) = default;
	constexpr ResultCopyConstructor& operator=(ResultCopyConstructor&&) = default;

	template <class ... Args>
	constexpr ResultCopyConstructor(bool has_val, Args&& ... args):
		data_(std::forward<Args>(args)...),
		has_value_(has_val)
	{
		
	}

	template <class T, class ... Args>
	constexpr ResultCopyConstructor(bool has_val, std::initializer_list<T> ilist, Args&& ... args):
		data_(ilist, std::forward<Args>(args)...),
		has_value_(has_val)
	{
		
	}

	constexpr const T& value() const { return base_.value(); }
	constexpr       T& value()       { return base_.value(); }

	constexpr const E& error() const { return base_.error(); }
	constexpr       E& error()       { return base_.error(); }

	constexpr const bool& has_value() const noexcept { return base_.has_value(); }
	constexpr bool&       has_value()       noexcept { return base_.has_value(); }

	constexpr void destruct_value() noexcept {
		return base_.destruct_value();
	}

	constexpr void destruct_error() noexcept {
		return base_.destruct_error();
	}

	constexpr void destruct() noexcept {
		return base_.destruct();
	}

private:
	base_type base_;
};

template <class T, class E>
struct ResultMoveConstructor<MemberStatus::Defaulted, T, E>:
	result_copy_constructor_type<T, E>
{
	static_assert(std::conjunction_v<std::is_trivially_move_constructible<Alts>...>);
	using base_type = result_copy_constructor_type<T, E>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::error;
	using base_type::destruct;
	using base_type::destruct_value;
	using base_type::destruct_error;

	constexpr ResultMoveConstructor() = default;
	constexpr ResultMoveConstructor(const ResultMoveConstructor&) = default;
	constexpr ResultMoveConstructor(ResultMoveConstructor&&) = default;
	constexpr ResultMoveConstructor& operator=(const ResultMoveConstructor&) = default;
	constexpr ResultMoveConstructor& operator=(ResultMoveConstructor&&) = default;
};

template <class T, class E>
struct ResultMoveConstructor<MemberStatus::Deleted, T, E>:
	result_copy_constructor_type<T, E>
{
	static_assert(std::disjunction_v<std::negation<std::is_move_constructible<Alts>>...>);
	using base_type = result_copy_constructor_type<T, E>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::error;
	using base_type::destruct;
	using base_type::destruct_value;
	using base_type::destruct_error;

	constexpr ResultMoveConstructor() = default;
	constexpr ResultMoveConstructor(const ResultMoveConstructor&) = default;
	constexpr ResultMoveConstructor(ResultMoveConstructor&&) = delete;
	constexpr ResultMoveConstructor& operator=(const ResultMoveConstructor&) = default;
	constexpr ResultMoveConstructor& operator=(ResultMoveConstructor&&) = default;
};

template <class T, class E>
struct ResultMoveConstructor<MemberStatus::Defined, T, E> {
	using base_type = result_copy_constructor_type<T, E>;
	
	constexpr ResultMoveConstructor() = default;
	constexpr ResultMoveConstructor(const ResultMoveConstructor&) = default;
	constexpr ResultMoveConstructor(ResultMoveConstructor&& other) noexcept(
		std::conjunction_v<std::is_nothrow_move_constructible<Alts>...>
	):
		base_([&]{
			if(other.has_value()) {
				return base_type(true, value_tag, std::move(other.value));
			} else {
				return base_type(false, error_tag, std::move(other.error));
			}
		}())
	{
		
	}

	constexpr ResultMoveConstructor& operator=(const ResultMoveConstructor&) = default;
	constexpr ResultMoveConstructor& operator=(ResultMoveConstructor&&) = default;

	template <class ... Args>
	constexpr ResultCopyConstructor(bool has_val, Args&& ... args):
		data_(std::forward<Args>(args)...),
		has_value_(has_val)
	{
		
	}

	template <class T, class ... Args>
	constexpr ResultCopyConstructor(bool has_val, std::initializer_list<T> ilist, Args&& ... args):
		data_(ilist, std::forward<Args>(args)...),
		has_value_(has_val)
	{
		
	}

	constexpr const T& value() const { return base_.value(); }
	constexpr       T& value()       { return base_.value(); }

	constexpr const E& error() const { return base_.error(); }
	constexpr       E& error()       { return base_.error(); }

	constexpr const bool& has_value() const noexcept { return base_.has_value(); }
	constexpr bool&       has_value()       noexcept { return base_.has_value(); }

	constexpr void destruct_value() noexcept {
		return base_.destruct_value();
	}

	constexpr void destruct_error() noexcept {
		return base_.destruct_error();
	}

	constexpr void destruct() noexcept {
		return base_.destruct();
	}

private:
	base_type base_;
};

template <class T, class E>
struct ResultCopyAssign<MemberStatus::Defaulted, T, E>:
	result_move_constructor_type<T, E>
{
	using base_type = result_move_constructor_type<T, E>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::error;
	using base_type::destruct;
	using base_type::destruct_value;
	using base_type::destruct_error;

	constexpr ResultCopyAssign() = default;
	constexpr ResultCopyAssign(const ResultCopyAssign&) = default;
	constexpr ResultCopyAssign(ResultCopyAssign&&) = default;
	constexpr ResultCopyAssign& operator=(const ResultCopyAssign&) = default;
	constexpr ResultCopyAssign& operator=(ResultCopyAssign&&) = default;
};

template <class T, class E>
struct ResultCopyAssign<MemberStatus::Deleted, T, E>:
	result_move_constructor_type<T, E>
{
	using base_type = result_move_constructor_type<T, E>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::error;
	using base_type::destruct;
	using base_type::destruct_value;
	using base_type::destruct_error;

	constexpr ResultCopyAssign() = default;
	constexpr ResultCopyAssign(const ResultCopyAssign&) = default;
	constexpr ResultCopyAssign(ResultCopyAssign&&) = default;
	constexpr ResultCopyAssign& operator=(const ResultCopyAssign&) = delete;
	constexpr ResultCopyAssign& operator=(ResultCopyAssign&&) = default;
};

template <class T, class E>
struct ResultCopyAssign<MemberStatus::Defined, T, E>:
	result_move_constructor_type<T, E>
{
	using base_type = result_move_constructor_type<T, E>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::error;
	using base_type::destruct;
	using base_type::destruct_value;
	using base_type::destruct_error;
	
	constexpr ResultCopyAssign() = default;
	constexpr ResultCopyAssign(const ResultCopyAssign& other) = default;
	constexpr ResultCopyAssign(ResultCopyAssign&&) = default;
	constexpr ResultCopyAssign& operator=(const ResultCopyAssign& other) noexcept(
		std::disjunction_v<detail::is_cv_void<T>, std::is_nothrow_copy_constructible<T>>
		&& std::disjunction_v<detail::is_cv_void<T>, std::is_nothrow_copy_assignable<T>>
		&& std::is_nothrow_copy_constructible_v<E>
		&& std::is_nothrow_copy_assignable_v<E>
	) {
		if(this->has_value()) {
			if(other.has_value()) {
				copy_assign_case(other, std::true_type{}, std::true_type[]);
			} else {
				copy_assign_case(other, std::true_type{}, std::false_type[]);
			}
		} else {
			if(other.has_value()) {
				copy_assign_case(other, std::false_type{}, std::true_type[]);
			} else {
				copy_assign_case(other, std::false_type{}, std::false_type[]);
			}
		}
		return *this;
	}

	constexpr ResultCopyAssign& operator=(ResultCopyAssign&&) = default;

private:
	constexpr void copy_assign_case(const ResultCopyAssign& other, std::true_type, std::true_type) {
		if constexpr(!is_cv_void_v<T>) {
			value() = other.value();
		}
	}

	constexpr void copy_assign_case(const ResultCopyAssign& other, std::false_type, std::false_type) {
		error() = other.error();
	}

	constexpr void copy_assign_case(const ResultCopyAssign& other, std::false_type, std::true_type) {
		if constexpr(is_cv_void_v<T>) {
			destruct_error();
		} else if constexpr(std::is_nothrow_copy_constructible_v<T>) {
			destruct_error();
			new (std::addresof(this->value())) T(other.value());
		} else if constexpr(std::is_nothrow_move_constructible_v<T>) {
			T tmp(other.value());
			destruct_error();
			new (std::addresof(this->value())) T(std::move(tmp));
		} else {
			static_assert(std::is_nothrow_move_constructible_v<E>);
			E tmp(std::move(this->error()));
			destruct_error();
			auto guard = make_manual_scope_guard([&](){
				new (std::addressof(this->error())) E(std::move(tmp));
			});
			new (std::addressof(this->value())) T(other.value());
			gaurd.active = false;
		}
		has_value() = true;
	}

	constexpr void copy_assign_case(const ResultCopyAssign& other, std::true_type, std::false_type) {
		if constexpr(is_cv_void_v<T>) {
			new (std::adderssof(this->error()) E(other.error());
		} else if constexpr(std::is_nothrow_copy_constructible_v<E>) {
			destruct_value();
			new (std::adderssof(this->error()) E(other.error());
		} else if constexpr(std::is_nothrow_move_constructible_v<T>) {
			E tmp(other.error());
			destruct_value();
			new (std::addresof(this->value())) E(std::move(tmp));
		} else {
			static_assert(std::is_nothrow_move_constructible_v<T>);
			T tmp(std::move(this->value()));
			destruct_value();
			auto guard = make_manual_scope_guard([&](){
				new (std::addressof(this->value())) T(std::move(tmp));
			});
			new (std::addressof(this->error())) E(other.error());
			gaurd.active = false;
		}
		has_value() = false;
	}
};

template <class T, class E>
struct ResultMoveAssign<MemberStatus::Defaulted, T, E>:
	result_copy_assign_type<T, E>
{
	using base_type = result_copy_assign_type<T, E>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::error;
	using base_type::destruct;
	using base_type::destruct_value;
	using base_type::destruct_error;

	constexpr ResultMoveAssign() = default;
	constexpr ResultMoveAssign(const ResultMoveAssign&) = default;
	constexpr ResultMoveAssign(ResultMoveAssign&&) = default;
	constexpr ResultMoveAssign& operator=(const ResultMoveAssign&) = default;
	constexpr ResultMoveAssign& operator=(ResultMoveAssign&&) = default;
};

template <class T, class E>
struct ResultMoveAssign<MemberStatus::Deleted, T, E>:
	result_copy_assign_type<T, E>
{
	using base_type = result_copy_assign_type<T, E>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::error;
	using base_type::destruct;
	using base_type::destruct_value;
	using base_type::destruct_error;

	constexpr ResultMoveAssign() = default;
	constexpr ResultMoveAssign(const ResultMoveAssign&) = default;
	constexpr ResultMoveAssign(ResultMoveAssign&&) = default;
	constexpr ResultMoveAssign& operator=(const ResultMoveAssign&) = default;
	constexpr ResultMoveAssign& operator=(ResultMoveAssign&&) = delete;
};

template <class T, class E>
struct ResultMoveAssign<MemberStatus::Defined, T, E>:
	result_copy_assign_type<T, E>
{
	using base_type = result_copy_assign_type<T, E>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::error;
	using base_type::destruct;
	using base_type::destruct_value;
	using base_type::destruct_error;
	
	constexpr ResultMoveAssign() = default;
	constexpr ResultMoveAssign(const ResultMoveAssign& other) = default;
	constexpr ResultMoveAssign(ResultMoveAssign&&) = default;
	constexpr ResultMoveAssign& operator=(const ResultMoveAssign&) = default;
	constexpr ResultMoveAssign& operator=(ResultMoveAssign&& other) noexcept(
		std::disjunction_v<detail::is_cv_void<T>, std::is_nothrow_move_constructible<T>>
		&& std::disjunction_v<detail::is_cv_void<T>, std::is_nothrow_move_assignable<T>>
		&& std::is_nothrow_move_constructible_v<E>
		&& std::is_nothrow_move_assignable_v<E>
	) {
		if(this->has_value()) {
			if(other.has_value()) {
				move_assign_case(other, std::true_type{}, std::true_type[]);
			} else {
				move_assign_case(other, std::true_type{}, std::false_type[]);
			}
		} else {
			if(other.has_value()) {
				move_assign_case(other, std::false_type{}, std::true_type[]);
			} else {
				move_assign_case(other, std::false_type{}, std::false_type[]);
			}
		}
		return *this;
	}
private:
	constexpr void move_assign_case(const ResultCopyAssign& other, std::true_type, std::true_type) {
		if constexpr(!is_cv_void_v<T>) {
			value() = std::move(other.value());
		}
	}

	constexpr void move_assign_case(const ResultCopyAssign& other, std::false_type, std::false_type) {
		error() = std::move(other.error());
	}

	constexpr void move_assign_case(const ResultCopyAssign& other, std::false_type, std::true_type) {
		if constexpr(is_cv_void_v<T>) {
			destruct_error();
		} else if constexpr(std::is_nothrow_move_constructible_v<T>) {
			destruct_error();
			new (std::addresof(this->value())) T(std::move(other.value()));
		} else {
			static_assert(
				std::is_nothrow_move_constructible_v<E>,
				"The move assignment operator should be defined as deleted!"
			);
			E tmp(std::move(this->error()));
			destruct_error();
			auto guard = make_manual_scope_guard([&](){
				new (std::addressof(this->error())) E(std::move(tmp));
			});
			new (std::addressof(this->value())) T(std::move(other.value()));
			gaurd.active = false;
		}
		has_value() = true;
	}

	constexpr void move_assign_case(const ResultCopyAssign& other, std::true_type, std::false_type) {
		if constexpr(is_cv_void_v<T>) {
			new (std::adderssof(this->error()) E(std::move(other.error()));
		} else if constexpr(std::is_nothrow_move_constructible_v<E>) {
			destruct_value();
			new (std::adderssof(this->error()) E(std::move(other.error()));
		} else {
			static_assert(std::is_nothrow_move_constructible_v<T>);
			T tmp(std::move(this->value()));
			destruct_value();
			auto guard = make_manual_scope_guard([&](){
				new (std::addressof(this->value())) T(std::move(tmp));
			});
			new (std::addressof(this->error())) E(other.error());
			gaurd.active = false;
		}
		has_value() = false;
	}
};

} /* namespace detail */


template <class E>
struct Error {
private:
	using detail::remove_cvref_t;
public:
	Error() = delete;
	Error(const Error&) = default;
	Error(Error&&) = default;

	template <
		class Err,
		std::enable_if_t<
			std::is_constructible_v<E, Err>
			&& !std::is_same_v<std::decay_t<Err>, std::in_place_t>
			&& !std::is_same_v<std::decay_t<Err>, Error>,
			bool
		> = false
	>
	constexpr explicit Error(Err&& err) noexcept(std::is_nothrow_constructible_v<E, Err>):
		error_(std::forward<Err>(err))
	{
		
	}

	template <
		class ... Args,
		std::enable_if_t<std::is_constructible_v<E, Args...>, bool> = false
	>
	constexpr explicit Error(std::in_place_t, Args&& ... args) noexcept(
		std::is_nothrow_constructible_v<E, Args...>
	):
		error_(std::forward<Args>(args)...)
	{
		
	}

	template <
		class U,
		class ... Args,
		std::enable_if_t<std::is_constructible_v<E, std::initializer_list<U>, Args...>, bool> = false
	>
	constexpr explicit Error(std::in_place_t, std::initializer_list<U> ilist, Args&& ... args) noexcept(
		std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args...>
	):
		error_(ilist, std::forward<Args>(args)...)
	{
		
	}

	template <
		class Err = E,
		std::enable_if_t<
			!std::is_convertible_v<Err, E>
			&& std::is_constructible_v<E, Err>
			&& !std::is_constructible_v<E, Error<Err>&>
			&& !std::is_constructible_v<E, Error<Err>>
			&& !std::is_constructible_v<E, const Error<Err>&>
			&& !std::is_constructible_v<E, const Error<Err>>
			&& !std::is_convertible_v<Error<Err>&, E>
			&& !std::is_convertible_v<Error<Err>, E>
			&& !std::is_convertible_v<const Error<Err>&, E>
			&& !std::is_convertible_v<const Error<Err>, E>,
			bool
		> = false
	>
	explicit constexpr Error(const Error<Err>& err) noexcept(std::is_nothrow_constructible_v<E, const Err&>):
		error_(err.value())
	{
		
	}

	template <
		class Err = E,
		std::enable_if_t<
			std::is_convertible_v<Err, E>
			&& std::is_constructible_v<E, Err>
			&& !std::is_constructible_v<E, Error<Err>&>
			&& !std::is_constructible_v<E, Error<Err>>
			&& !std::is_constructible_v<E, const Error<Err>&>
			&& !std::is_constructible_v<E, const Error<Err>>
			&& !std::is_convertible_v<Error<Err>&, E>
			&& !std::is_convertible_v<Error<Err>, E>
			&& !std::is_convertible_v<const Error<Err>&, E>
			&& !std::is_convertible_v<const Error<Err>, E>,
			bool
		> = false
	>
	constexpr Error(const Error<Err>& err) noexcept(std::is_nothrow_constructible_v<E, const Err&>):
		error_(err.value())
	{
		
	}

	template <
		class Err = E,
		std::enable_if_t<
			!std::is_convertible_v<Err, E>
			&& std::is_constructible_v<E, Err>
			&& !std::is_constructible_v<E, Error<Err>&>
			&& !std::is_constructible_v<E, Error<Err>>
			&& !std::is_constructible_v<E, const Error<Err>&>
			&& !std::is_constructible_v<E, const Error<Err>>
			&& !std::is_convertible_v<Error<Err>&, E>
			&& !std::is_convertible_v<Error<Err>, E>
			&& !std::is_convertible_v<const Error<Err>&, E>
			&& !std::is_convertible_v<const Error<Err>, E>,
			bool
		> = false
	>
	explicit constexpr Error(Error<Err>&& err) noexcept(std::is_nothrow_constructible_v<E, Err&&>):
		error_(err.value())
	{
		
	}

	template <
		class Err = E,
		std::enable_if_t<
			std::is_convertible_v<Err, E>
			&& std::is_constructible_v<E, Err>
			&& !std::is_constructible_v<E, Error<Err>&>
			&& !std::is_constructible_v<E, Error<Err>>
			&& !std::is_constructible_v<E, const Error<Err>&>
			&& !std::is_constructible_v<E, const Error<Err>>
			&& !std::is_convertible_v<Error<Err>&, E>
			&& !std::is_convertible_v<Error<Err>, E>
			&& !std::is_convertible_v<const Error<Err>&, E>
			&& !std::is_convertible_v<const Error<Err>, E>,
			bool
		> = false
	>
	constexpr Error(Error<Err>&& err) noexcept(std::is_nothrow_constructible_v<E, Err&&>):
		error_(std::move(err.value()))
	{
		
	}

	Error& operator=(const Error&) = default;
	Error& operator=(Error&&) = default;

	template <
		class Err = E,
		std::enable_if_t<
			!std::is_same_v<Err, E>
			&& std::is_assignable_v<E, const Err&>,
			bool
		> = false
	>
	constexpr Error& operator=(const Error<Err>& err)
		noexcept(std::is_nothrow_assignable_v<E, const Err&>)
	{
		this->value() = err.value();
	}

	template <
		class Err,
		std::enable_if_t<
			!std::is_same_v<Err, E>
			&& std::is_assignable_v<E, Err&&>,
			bool
		> = false
	>
	constexpr Error& operator=(Error<Err>&& err)
		noexcept(std::is_nothrow_assignable_v<E, const Err&>)
	{
		this->value() = err.value();
	}

	constexpr const E&  value() const &  { return error_; }
	constexpr const E&& value() const && { return error_; }
	constexpr       E&  value()       &  { return error_; }
	constexpr       E&& value()       && { return error_; }

	constexpr void swap(Error& other) noexcept(std::is_nothrow_swappable_v<E>) {
		using std::swap;
		swap(this->value(), other.value());
	}

private:
	E error_;
};

template <class E>
constexpr void swap(Error<E>& lhs, Error<E>& rhs)
	noexcept(noexcept(lhs.swap(rhs)))
{
	lhs.swap(rhs);
}

template <class E1, class E2, class = decltype(std::declval<E1>() == std::declval<E2>())>
friend constexpr bool operator==(const Error<E1>& lhs, const Error<E2>& rhs)
	noexcept(noexcept(lhs.value() == rhs.value()))
{
	return lhs.value() == rhs.value();
}

template <class E1, class E2, class = decltype(std::declval<E1>() != std::declval<E2>())>
friend constexpr bool operator!=(const Error<E1>& lhs, const Error<E2>& rhs)
	noexcept(noexcept(lhs.value() != rhs.value()))
{
	return lhs.value() != rhs.value();
}

template <class T, class E>
struct Result {
private:
	using data_type = result_data_type<T, E>;
public:
	Result() = default;
	Result(const Result&) = default;
	Result(Result&&) = default;

	template <
		class U,
		class G,
		std::enable_if_t<
			!std::conjunction_v<
				std::disjunction<
					std::conjunction<
						detail::is_cv_void<T>,
						detail::is_cv_void<U>
					>,
					std::is_convertible<const U&, T>
				>,
				std::is_convertible<const G&, E>,
			>
			&& std::conjunction_v<
				std::disjunction<
					std::conjunction<
						detail::is_cv_void<T>,
						detail::is_cv_void<U>
					>,
					std::conjunction<
						std::is_constructible_v<T, const U&>,
						std::negation<std::is_constructible<T, Result<U, G>&>>,
						std::negation<std::is_constructible<T, Result<U, G>&&>>,
						std::negation<std::is_constructible<T, const Result<U, G>&>>,
						std::negation<std::is_constructible<T, const Result<U, G>&&>>,
						std::negation<std::is_convertible<Result<U, G>&, T>>,
						std::negation<std::is_convertible<Result<U, G>&&, T>>,
						std::negation<std::is_convertible<const Result<U, G>&, T>>,
						std::negation<std::is_convertible<const Result<U, G>&&, T>>
					>
				>,
				std::is_constructible<E, const G&>,
				std::negation<std::is_constructible<Error<E>, Result<U, G>&>>,
				std::negation<std::is_constructible<Error<E>, Result<U, G>&&>>,
				std::negation<std::is_constructible<Error<E>, const Result<U, G>&>>,
				std::negation<std::is_constructible<Error<E>, const Result<U, G>&&>>,
				std::negation<std::is_convertible<Result<U, G>&, Error<E>>>,
				std::negation<std::is_convertible<Result<U, G>&&, Error<E>>>,
				std::negation<std::is_convertible<const Result<U, G>&, Error<E>>>,
				std::negation<std::is_convertible<const Result<U, G>&&, Error<E>>>
			>,
			bool
		> = false
	>
	explicit constexpr Result(const Result<U, G>& other) noexcept(
		std::is_nothrow_constructible_v<T, const U&>
		&& std::is_nothrow_constructible_v<E, const G&>
	):
		data_([&]() -> data_type {
			if(other.has_value()) {
				if constexpr(detail::is_cv_void_v<T>) {
					return data_type(true, value_tag);
				} else {
					return data_type(true, value_tag, other.value());
				}
			} else {
				return data_type(false, error_tag, other.error());
			}
		}())
	{
		
	}

	template <
		class U,
		class G,
		std::enable_if_t<
			std::conjunction_v<
				std::disjunction<
					std::conjunction<
						detail::is_cv_void<T>,
						detail::is_cv_void<U>
					>,
					std::is_convertible<const U&, T>
				>,
				std::is_convertible<const G&, E>,
			>
			&& std::conjunction_v<
				std::disjunction<
					std::conjunction<
						detail::is_cv_void<T>,
						detail::is_cv_void<U>
					>,
					std::conjunction<
						std::is_constructible_v<T, const U&>,
						std::negation<std::is_constructible<T, Result<U, G>&>>,
						std::negation<std::is_constructible<T, Result<U, G>&&>>,
						std::negation<std::is_constructible<T, const Result<U, G>&>>,
						std::negation<std::is_constructible<T, const Result<U, G>&&>>,
						std::negation<std::is_convertible<Result<U, G>&, T>>,
						std::negation<std::is_convertible<Result<U, G>&&, T>>,
						std::negation<std::is_convertible<const Result<U, G>&, T>>,
						std::negation<std::is_convertible<const Result<U, G>&&, T>>
					>
				>,
				std::is_constructible<E, const G&>,
				std::negation<std::is_constructible<Error<E>, Result<U, G>&>>,
				std::negation<std::is_constructible<Error<E>, Result<U, G>&&>>,
				std::negation<std::is_constructible<Error<E>, const Result<U, G>&>>,
				std::negation<std::is_constructible<Error<E>, const Result<U, G>&&>>,
				std::negation<std::is_convertible<Result<U, G>&, Error<E>>>,
				std::negation<std::is_convertible<Result<U, G>&&, Error<E>>>,
				std::negation<std::is_convertible<const Result<U, G>&, Error<E>>>,
				std::negation<std::is_convertible<const Result<U, G>&&, Error<E>>>
			>,
			bool
		> = false
	>
	constexpr Result(const Result<U, G>& other) noexcept(
		std::is_nothrow_constructible_v<T, const U&>
		&& std::is_nothrow_constructible_v<E, const G&>
	):
		data_([&]() -> data_type {
			if(other.has_value()) {
				if constexpr(detail::is_cv_void_v<T>) {
					return data_type(true, value_tag);
				} else {
					return data_type(true, value_tag, other.value());
				}
			} else {
				return data_type(false, error_tag, other.error());
			}
		}())
	{
		
	}

	template <
		class U,
		class G,
		std::enable_if_t<
			!std::conjunction_v<
				std::disjunction<
					std::conjunction<
						detail::is_cv_void<T>,
						detail::is_cv_void<U>
					>,
					std::is_convertible<U&&, T>
				>,
				std::is_convertible<G&&, E>,
			>
			&& std::conjunction_v<
				std::disjunction<
					std::conjunction<
						detail::is_cv_void<T>,
						detail::is_cv_void<U>
					>,
					std::conjunction<
						std::is_constructible<T, U&&>,
						std::negation<std::is_constructible<T, Result<U, G>&>>,
						std::negation<std::is_constructible<T, Result<U, G>&&>>,
						std::negation<std::is_constructible<T, const Result<U, G>&>>,
						std::negation<std::is_constructible<T, const Result<U, G>&&>>,
						std::negation<std::is_convertible<Result<U, G>&, T>>,
						std::negation<std::is_convertible<Result<U, G>&&, T>>,
						std::negation<std::is_convertible<const Result<U, G>&, T>>,
						std::negation<std::is_convertible<const Result<U, G>&&, T>>
					>
				>,
				std::is_constructible<E, G&&>,
				std::negation<std::is_constructible<Error<E>, Result<U, G>&>>,
				std::negation<std::is_constructible<Error<E>, Result<U, G>&&>>,
				std::negation<std::is_constructible<Error<E>, const Result<U, G>&>>,
				std::negation<std::is_constructible<Error<E>, const Result<U, G>&&>>,
				std::negation<std::is_convertible<Result<U, G>&, Error<E>>>,
				std::negation<std::is_convertible<Result<U, G>&&, Error<E>>>,
				std::negation<std::is_convertible<const Result<U, G>&, Error<E>>>,
				std::negation<std::is_convertible<const Result<U, G>&&, Error<E>>>
			>,
			bool
		> = false
	>
	constexpr explicit Result(const Result<U, G>& other) noexcept(
		std::is_nothrow_constructible_v<T, const U&>
		&& std::is_nothrow_constructible_v<E, const G&>
	):
		data_([&]() -> data_type {
			if(other.has_value()) {
				if constexpr(detail::is_cv_void_v<T>) {
					return data_type(true, value_tag);
				} else {
					return data_type(true, value_tag, std::move(other.value()));
				}
			} else {
				return data_type(false, error_tag, std::move(other.error()));
			}
		}())
	{
		
	}
	
	template <
		class U,
		class G,
		std::enable_if_t<
			std::conjunction_v<
				std::disjunction<
					std::conjunction<
						detail::is_cv_void<T>,
						detail::is_cv_void<U>
					>,
					std::is_convertible<U&&, T>
				>,
				std::is_convertible<G&&, E>,
			>
			&& std::conjunction_v<
				std::disjunction<
					std::conjunction<
						detail::is_cv_void<T>,
						detail::is_cv_void<U>
					>,
					std::conjunction<
						std::is_constructible<T, U&&>,
						std::negation<std::is_constructible<T, Result<U, G>&>>,
						std::negation<std::is_constructible<T, Result<U, G>&&>>,
						std::negation<std::is_constructible<T, const Result<U, G>&>>,
						std::negation<std::is_constructible<T, const Result<U, G>&&>>,
						std::negation<std::is_convertible<Result<U, G>&, T>>,
						std::negation<std::is_convertible<Result<U, G>&&, T>>,
						std::negation<std::is_convertible<const Result<U, G>&, T>>,
						std::negation<std::is_convertible<const Result<U, G>&&, T>>
					>
				>,
				std::is_constructible<E, G&&>,
				std::negation<std::is_constructible<Error<E>, Result<U, G>&>>,
				std::negation<std::is_constructible<Error<E>, Result<U, G>&&>>,
				std::negation<std::is_constructible<Error<E>, const Result<U, G>&>>,
				std::negation<std::is_constructible<Error<E>, const Result<U, G>&&>>,
				std::negation<std::is_convertible<Result<U, G>&, Error<E>>>,
				std::negation<std::is_convertible<Result<U, G>&&, Error<E>>>,
				std::negation<std::is_convertible<const Result<U, G>&, Error<E>>>,
				std::negation<std::is_convertible<const Result<U, G>&&, Error<E>>>
			>,
			bool
		> = false
	>
	constexpr Result(const Result<U, G>& other) noexcept(
		std::is_nothrow_constructible_v<T, const U&>
		&& std::is_nothrow_constructible_v<E, const G&>
	):
		data_([&]() -> data_type {
			if(other.has_value()) {
				if constexpr(detail::is_cv_void_v<T>) {
					return data_type(true, value_tag);
				} else {
					return data_type(true, value_tag, std::move(other.value()));
				}
			} else {
				return data_type(false, error_tag, std::move(other.error()));
			}
		}())
	{
		
	}
	
	template <
		class U = T,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<detail::is_cv_void<T>>,
				std::negation<std::is_convertible<U&&, T>>,
				std::constructible<T, U&&>,
				std::negation<std::is_same<std::decay_t<U>, std::in_place_t>>,
				std::negation<std::is_same<std::decay_t<U>, Result>>,
				std::negation<std::is_same<std::decay_t<U>, Error<E>>>
			>,
			bool
		> = false
	>
	constexpr explicit Result(U&& v) noexcept(std::is_nothrow_constructible_v<T, U&&>):
		data_(true, value_tag, std::forward<U>(v))
	{
			
	}

	template <
		class U = T,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<detail::is_cv_void<T>>,
				std::is_convertible<U&&, T>,
				std::constructible<T, U&&>,
				std::negation<std::is_same<std::decay_t<U>, std::in_place_t>>,
				std::negation<std::is_same<std::decay_t<U>, Result>>,
				std::negation<std::is_same<std::decay_t<U>, Error<E>>>
			>,
			bool
		> = false
	>
	constexpr Result(U&& v) noexcept(std::is_nothrow_constructible_v<T, U&&>):
		data_(true, value_tag, std::forward<U>(v))
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::is_convertible<const G&, E>,
				std::constructible<E, const G&>
			>,
			bool
		> = false
	>
	constexpr Result(const Error<G>& v) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_(true, error_tag, v.value())
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<std::is_convertible<const G&, E>>,
				std::constructible<E, const G&>
			>,
			bool
		> = false
	>
	constexpr explicit Result(const Error<G>& v) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_(true, error_tag, v.value())
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::is_convertible<G&&, E>,
				std::constructible<E, G&&>
			>,
			bool
		> = false
	>
	constexpr Result(Error<G>&& v) noexcept(std::is_nothrow_constructible_v<E, G&>):
		data_(true, error_tag, std::move(v.value()))
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<std::is_convertible<G&&, E>>,
				std::constructible<E, G&&>
			>,
			bool
		> = false
	>
	constexpr explicit Result(Error<G>&& v) noexcept(std::is_nothrow_constructible_v<E, G&&>):
		data_(true, error_tag, std::move(v.value()))
	{
			
	}

	template <
		class Args ...,
		std::enable_if_t<
			std::conditional_t<
				detail::is_cv_void_v<T>,
				std::bool_constant<(sizeof...(Args) == 0)>,
				std::is_constructible<T, Args&&...>
			>::value
			bool
		> = false
	>
	constexpr explicit Result(std::in_place_t, Args&& ... args) noexcept(std::is_nothrow_constructible_v<E, G&&>):
		data_(true, value_tag, std::forward<Args>(args)...)
	{
			
	}

	template <
		class U,
		class Args ...,
		std::enable_if_t<
			!detail::is_cv_void_v<T>
			std::is_constructible<T, std::initializer_list<U>&, Args&&...>
			>::value
			bool
		> = false
	>
	constexpr explicit Result(std::in_place_t, std::initializer_list<U> ilist, Args&& ... args) noexcept(
		std::is_nothrow_constructible_v<E, G&&>
	):
		data_(true, value_tag, ilist, std::forward<Args>(args)...)
	{
			
	}


	Result& operator=(const Result&) = default;
	Result& operator=(Result&&) = default;

private:
	data_type data_;
};

} /* inline namespace result */

} /* namespace tim */

#endif /* TIM_RESULT_HPP */

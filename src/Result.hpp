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
		std::conditional_t<std::is_same_v<T, void>, EmptyAlternative, T> value;
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
		std::conditional_t<std::is_same_v<T, void>, EmptyAlternative, T> value;
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
		std::conditional_t<std::is_same_v<T, void>, EmptyAlternative, T> value;
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
		if constexpr(!std::is_same_v<T, void> && !std::is_trivially_destructible_v<T>) {
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
	bool has_value_;
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
			std::is_same<std::remove_cv_t<T>, void>,
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
			std::is_same<std::remove_cv_t<T>, void>,
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
				std::is_same<std::remove_cv_t<T>, void>,
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
				std::is_same<std::remove_cv_t<T>, void>,
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
			std::is_same<std::remove_cv_t<T>, void>,
			std::is_copy_assignable<E>,
			std::is_copy_constructible<E>
		>,
		std::conjunction<
			std::negation<std::is_same<std::remove_cv_t<T>, void>>,
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
				std::is_same<std::remove_cv_t<T>, void>,
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
			std::is_same<std::remove_cv_t<T>, void>,
			std::is_move_assignable<E>,
			std::is_move_constructible<E>
		>,
		std::conjunction<
			std::negation<std::is_same<std::remove_cv_t<T>, void>>,
			std::is_move_assignable<T>,
			std::is_move_constructible<T>,
			std::is_nothrow_move_assignable<E>,
			std::is_nothrow_move_constructible<E>,
		>
	>,
	std::conditional_t<
		std::conjunction_v<
			std::disjunction<
				std::is_same<std::remove_cv_t<T>, void>,
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
struct ResultDestructor<MemberStatus::Defaulted, Alts ...>:
	ResultBaseMethods<T, E>
{
	using base_type = ResultBaseMethods<T, E>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::error;
	using base_type::destruct;

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
	
	constexpr ResultCopyAssign() = default;
	constexpr ResultCopyAssign(const ResultCopyAssign& other) = default;
	constexpr ResultCopyAssign(ResultCopyAssign&&) = default;
	constexpr ResultCopyAssign& operator=(const ResultCopyAssign& other) {
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
		if constexpr(!std::is_same_v<T, void>) {
			value() = other.value();
		}
	}

	constexpr void copy_assign_case(const ResultCopyAssign& other, std::false_type, std::false_type) {
		error() = other.error();
	}

	constexpr void copy_assign_case(const ResultCopyAssign& other, std::false_type, std::true_type) {
		if constexpr(std::is_same_v<T, void>) {
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
		if constexpr(std::is_same_v<T, void>) {
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
	
	constexpr ResultMoveAssign() = default;
	constexpr ResultMoveAssign(const ResultMoveAssign& other) = default;
	constexpr ResultMoveAssign(ResultMoveAssign&&) = default;
	constexpr ResultMoveAssign& operator=(const ResultMoveAssign&) = default;
	constexpr ResultMoveAssign& operator=(ResultMoveAssign&& other) noexcept(
		std::conjunction_v<
			std::is_nothrow_move_constructible<Alts>...,
			std::is_nothrow_move_assignable<Alts>...
		>
	) {
		if(this != &other) {
			if constexpr(can_be_valueless()) {
				if(other.valueless_by_exception()) {
					this->make_valueless();
				} else {
					this->do_move_assign(std::move(other));
				}
			} else {
				assert(not this->valueless_by_exception());
				assert(not other.valueless_by_exception());
				this->do_move_assign(std::move(other));
			}
		}
		return *this;
	}
};



} /* namespace detail */

} /* inline namespace result */

} /* namespace tim */

#endif /* TIM_RESULT_HPP */

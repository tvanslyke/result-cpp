#ifndef TIM_RESULT_HPP
#define TIM_RESULT_HPP

#include <type_traits>
#include <optional>
#include <memory>

namespace tim {

inline namespace result {

template <class T, class E>
struct Result;

namespace detail {

template <class T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <class T>
using is_cv_void = std::is_same<std::remove_cv_t<T>, void>;

template <class T>
inline constexpr bool is_cv_void_v = is_cv_void<T>::value;

template <class T>
struct ManualScopeGuard {

	~ManualScopeGuard() {
		if(active) {
			action();
		}
	}

	T action;
	bool active;
};

template <class T>
ManualScopeGuard<std::decay_t<T>> make_manual_scope_guard(T&& action) {
	return ManualScopeGuard<std::decay_t<T>>{std::forward<T>(action), true};
}

struct EmptyAlternative {};

enum class MemberStatus {
	Defaulted,
	Deleted,
	Defined
};

template <MemberStatus S, class T, class E>
struct ResultUnionImpl;

using value_tag_t = std::in_place_index_t<0>;
using error_tag_t = std::in_place_index_t<1>;

inline constexpr value_tag_t value_tag = value_tag_t{};
inline constexpr error_tag_t error_tag = error_tag_t{};

template <class T, class E>
struct ResultUnionImpl<MemberStatus::Defaulted, T, E> {

	constexpr ResultUnionImpl() = default;
	constexpr ResultUnionImpl(const ResultUnionImpl&) = default;
	constexpr ResultUnionImpl(ResultUnionImpl&&) = default;
	constexpr ResultUnionImpl& operator=(const ResultUnionImpl&) = default;
	constexpr ResultUnionImpl& operator=(ResultUnionImpl&&) = default;

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
		std::conditional_t<is_cv_void_v<T>, EmptyAlternative, T> value;
		E error;
		EmptyAlternative hidden_;
	};
};

template <class T, class E>
struct ResultUnionImpl<MemberStatus::Deleted, T, E> {

	constexpr ResultUnionImpl() = default;
                  
	constexpr ResultUnionImpl(const ResultUnionImpl&) = default;
	constexpr ResultUnionImpl(ResultUnionImpl&&) = default;

	constexpr ResultUnionImpl& operator=(const ResultUnionImpl&) = default;
	constexpr ResultUnionImpl& operator=(ResultUnionImpl&&) = default;

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
		std::conditional_t<is_cv_void_v<T>, EmptyAlternative, T> value;
		E error;
		EmptyAlternative hidden_;
	};
};

template <class T, class E>
struct ResultUnionImpl<MemberStatus::Defined, T, E> {

	constexpr ResultUnionImpl() = default;
                     
	constexpr ResultUnionImpl(const ResultUnionImpl&) = default;
	constexpr ResultUnionImpl(ResultUnionImpl&&) = default;

	constexpr ResultUnionImpl& operator=(const ResultUnionImpl&) = default;
	constexpr ResultUnionImpl& operator=(ResultUnionImpl&&) = default;

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
		std::conditional_t<is_cv_void_v<T>, EmptyAlternative, T> value;
		E error;
		EmptyAlternative hidden_;
	};
};

template <class T, class E>
using ResultUnion = std::conditional_t<
	std::conjunction_v<
		std::disjunction<
			detail::is_cv_void<T>,
			std::is_destructible<T>
		>,
		std::is_destructible<E>
	>,
	std::conditional_t<
		std::conjunction_v<
			std::disjunction<
				detail::is_cv_void<T>,
				std::is_trivially_destructible<T>
			>,
			std::is_trivially_destructible<E>
		>,
		ResultUnionImpl<MemberStatus::Defaulted, T, E>,
		ResultUnionImpl<MemberStatus::Defined, T, E>
	>,
	ResultUnionImpl<MemberStatus::Deleted, T, E>
>;

template <class T, class E>
struct ResultBaseMethods {
	using value_type = std::conditional_t<is_cv_void_v<T>, EmptyAlternative, T>;

	constexpr ResultBaseMethods() = default;

	template <class ... Args>
	constexpr ResultBaseMethods(value_tag_t, Args&& ... args):
		data_(value_tag, std::forward<Args>(args)...),
		has_value_(true)
	{
		
	}

	template <class U, class ... Args>
	constexpr ResultBaseMethods(value_tag_t, std::initializer_list<U> ilist, Args&& ... args):
		data_(value_tag, ilist, std::forward<Args>(args)...),
		has_value_(true)
	{
		
	}

	template <class ... Args>
	constexpr ResultBaseMethods(error_tag_t, Args&& ... args):
		data_(error_tag, std::forward<Args>(args)...),
		has_value_(false)
	{
		
	}

	template <class U, class ... Args>
	constexpr ResultBaseMethods(error_tag_t, std::initializer_list<U> ilist, Args&& ... args):
		data_(error_tag, ilist, std::forward<Args>(args)...),
		has_value_(false)
	{
		
	}

	constexpr const value_type& value() const { return *std::launder(std::addressof(data_.value)); }
	constexpr       value_type& value()       { return *std::launder(std::addressof(data_.value)); }

	constexpr const E& error() const { return *std::launder(std::addressof(data_.error)); }
	constexpr       E& error()       { return *std::launder(std::addressof(data_.error)); }

	constexpr const bool& has_value() const noexcept { return has_value_; }
	constexpr bool&       has_value()       noexcept { return has_value_; }

	constexpr void destruct_value() noexcept {
		if constexpr(
			std::conjunction_v<
				std::negation<is_cv_void<T>>,
				std::negation<std::is_trivially_destructible<T>>
			>
		) {
			std::destroy_at(std::addressof(data_.value));
		}
	}

	constexpr void destruct_error() noexcept {
		if constexpr(!std::is_trivially_destructible_v<E>) {
			std::destroy_at(std::addressof(data_.error));
		}
	}

	constexpr void destruct() noexcept {
		if(has_value()) {
			destruct_value();
		} else {
			destruct_error();
		}
	}

private:
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
	std::disjunction_v<
		detail::is_cv_void<T>,
		std::is_default_constructible<T>
	>,
	std::conditional_t<
		// Don't actually want trivial default initialization.
		// std::conjunction_v<
		// 	std::disjunction<
		// 		detail::is_cv_void<T>,
		// 		std::is_trivially_default_constructible<T>
		// 	>,
		// 	std::is_trivially_default_constructible<E>
		// >,
		false,
		ResultDefaultConstructor<MemberStatus::Defaulted, T, E>,
		ResultDefaultConstructor<MemberStatus::Defined, T, E>
	>,
	ResultDefaultConstructor<MemberStatus::Deleted, T, E>
>;


template <class T, class E>
using result_copy_constructor_type = std::conditional_t<
	std::conjunction_v<
		std::disjunction<
			detail::is_cv_void<T>,
			std::is_copy_constructible<T>
		>,
		std::is_copy_constructible<E>
	>,
	std::conditional_t<
		std::conjunction_v<
			std::disjunction<
				detail::is_cv_void<T>,
				std::is_trivially_copy_constructible<T>
			>,
			std::is_trivially_copy_constructible<E>
		>,
		ResultCopyConstructor<MemberStatus::Defaulted, T, E>,
		ResultCopyConstructor<MemberStatus::Defined, T, E>
	>,
	ResultCopyConstructor<MemberStatus::Deleted, T, E>
>;

template <class T, class E>
using result_move_constructor_type = std::conditional_t<
	std::conjunction_v<
		std::disjunction<
			detail::is_cv_void<T>,
			std::is_move_constructible<T>
		>,
		std::is_move_constructible<E>
	>,
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
			std::disjunction<
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
			std::is_move_assignable<E>,
			std::is_move_constructible<E>,
			std::disjunction<
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
					std::is_trivially_move_assignable<T>,
					std::is_trivially_move_constructible<T>,
					std::is_trivially_destructible<T>
				>
			>,
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
struct ResultDestructor<MemberStatus::Defaulted, T, E>:
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
	
	constexpr ResultDefaultConstructor() noexcept(std::is_nothrow_default_constructible_v<T>):
		base_type(value_tag)
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
	using value_type = std::conditional_t<is_cv_void_v<T>, EmptyAlternative, T>;
	
	constexpr ResultCopyConstructor() = default;
	constexpr ResultCopyConstructor(const ResultCopyConstructor& other):
		base_([&other]{
			if(other.has_value()) {
				return base_type(value_tag, other.value());
			} else {
				return base_type(error_tag, other.error());
			}
		}())
	{
		
	}

	constexpr ResultCopyConstructor(ResultCopyConstructor&&) = default;

	constexpr ResultCopyConstructor& operator=(const ResultCopyConstructor&) = default;
	constexpr ResultCopyConstructor& operator=(ResultCopyConstructor&&) = default;

	template <class ... Args>
	constexpr ResultCopyConstructor(value_tag_t, Args&& ... args):
		base_(value_tag, std::forward<Args>(args)...)
	{
		
	}

	template <class U, class ... Args>
	constexpr ResultCopyConstructor(value_tag_t, std::initializer_list<U> ilist, Args&& ... args):
		base_(value_tag, ilist, std::forward<Args>(args)...)
	{
		
	}

	template <class ... Args>
	constexpr ResultCopyConstructor(error_tag_t, Args&& ... args):
		base_(error_tag, std::forward<Args>(args)...)
	{
		
	}

	template <class U, class ... Args>
	constexpr ResultCopyConstructor(error_tag_t, std::initializer_list<U> ilist, Args&& ... args):
		base_(error_tag, ilist, std::forward<Args>(args)...)
	{
		
	}

	constexpr const value_type& value() const { return base_.value(); }
	constexpr       value_type& value()       { return base_.value(); }

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
	static_assert(std::disjunction_v<detail::is_cv_void<T>, std::is_trivially_move_constructible<T>>);
	static_assert(std::is_trivially_move_constructible_v<E>);
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
	static_assert(
		(!std::disjunction_v<detail::is_cv_void<T>, std::is_trivially_move_constructible<T>>)
		|| !std::is_trivially_move_constructible_v<E>
	);
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
	using value_type = std::conditional_t<is_cv_void_v<T>, EmptyAlternative, T>;
	
	constexpr ResultMoveConstructor() = default;
	constexpr ResultMoveConstructor(const ResultMoveConstructor&) = default;
	constexpr ResultMoveConstructor(ResultMoveConstructor&& other) noexcept(
		std::conjunction_v<
			std::disjunction<
				detail::is_cv_void<T>,
				std::is_nothrow_move_constructible<T>
			>,
			std::is_nothrow_move_constructible<E>
		>
	):
		base_([&]{
			if(other.has_value()) {
				return base_type(value_tag, std::move(other.value()));
			} else {
				return base_type(error_tag, std::move(other.error()));
			}
		}())
	{
		
	}

	constexpr ResultMoveConstructor& operator=(const ResultMoveConstructor&) = default;
	constexpr ResultMoveConstructor& operator=(ResultMoveConstructor&&) = default;

	template <class ... Args>
	constexpr ResultMoveConstructor(value_tag_t, Args&& ... args):
		base_(value_tag, std::forward<Args>(args)...)
	{
		
	}

	template <class U, class ... Args>
	constexpr ResultMoveConstructor(value_tag_t, std::initializer_list<U> ilist, Args&& ... args):
		base_(value_tag, ilist, std::forward<Args>(args)...)
	{
		
	}

	template <class ... Args>
	constexpr ResultMoveConstructor(error_tag_t, Args&& ... args):
		base_(error_tag, std::forward<Args>(args)...)
	{
		
	}

	template <class U, class ... Args>
	constexpr ResultMoveConstructor(error_tag_t, std::initializer_list<U> ilist, Args&& ... args):
		base_(error_tag, ilist, std::forward<Args>(args)...)
	{
		
	}

	constexpr const value_type& value() const { return base_.value(); }
	constexpr       value_type& value()       { return base_.value(); }

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
				copy_assign_case(other, std::true_type{}, std::true_type{});
			} else {
				copy_assign_case(other, std::true_type{}, std::false_type{});
			}
		} else {
			if(other.has_value()) {
				copy_assign_case(other, std::false_type{}, std::true_type{});
			} else {
				copy_assign_case(other, std::false_type{}, std::false_type{});
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
			new (std::addressof(this->value())) T(other.value());
		} else if constexpr(std::is_nothrow_move_constructible_v<T>) {
			T tmp(other.value());
			destruct_error();
			new (std::addressof(this->value())) T(std::move(tmp));
		} else {
			static_assert(std::is_nothrow_move_constructible_v<E>);
			E tmp(std::move(this->error()));
			destruct_error();
			auto guard = make_manual_scope_guard([&](){
				new (std::addressof(this->error())) E(std::move(tmp));
			});
			new (std::addressof(this->value())) T(other.value());
			guard.active = false;
		}
		has_value() = true;
	}

	constexpr void copy_assign_case(const ResultCopyAssign& other, std::true_type, std::false_type) {
		if constexpr(is_cv_void_v<T>) {
			new (std::addressof(this->error())) E(other.error());
		} else if constexpr(std::is_nothrow_copy_constructible_v<E>) {
			destruct_value();
			new (std::addressof(this->error())) E(other.error());
		} else if constexpr(std::is_nothrow_move_constructible_v<T>) {
			E tmp(other.error());
			destruct_value();
			new (std::addressof(this->value())) E(std::move(tmp));
		} else {
			static_assert(std::is_nothrow_move_constructible_v<T>);
			T tmp(std::move(this->value()));
			destruct_value();
			auto guard = make_manual_scope_guard([&](){
				new (std::addressof(this->value())) T(std::move(tmp));
			});
			new (std::addressof(this->error())) E(other.error());
			guard.active = false;
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
				move_assign_case(std::move(other), std::true_type{}, std::true_type{});
			} else {
				move_assign_case(std::move(other), std::true_type{}, std::false_type{});
			}
		} else {
			if(other.has_value()) {
				move_assign_case(std::move(other), std::false_type{}, std::true_type{});
			} else {
				move_assign_case(std::move(other), std::false_type{}, std::false_type{});
			}
		}
		return *this;
	}
private:
	constexpr void move_assign_case(ResultMoveAssign&& other, std::true_type, std::true_type) {
		if constexpr(!is_cv_void_v<T>) {
			value() = std::move(other.value());
		}
	}

	constexpr void move_assign_case(ResultMoveAssign&& other, std::false_type, std::false_type) {
		error() = std::move(other.error());
	}

	constexpr void move_assign_case(ResultMoveAssign&& other, std::false_type, std::true_type) {
		if constexpr(is_cv_void_v<T>) {
			destruct_error();
		} else if constexpr(std::is_nothrow_move_constructible_v<T>) {
			destruct_error();
			new (std::addressof(this->value())) T(std::move(other.value()));
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
			guard.active = false;
		}
		has_value() = true;
	}

	constexpr void move_assign_case(ResultMoveAssign&& other, std::true_type, std::false_type) {
		if constexpr(is_cv_void_v<T>) {
			new (std::addressof(this->error())) E(std::move(other.error()));
		} else if constexpr(std::is_nothrow_move_constructible_v<E>) {
			destruct_value();
			new (std::addressof(this->error())) E(std::move(other.error()));
		} else {
			static_assert(std::is_nothrow_move_constructible_v<T>);
			T tmp(std::move(this->value()));
			destruct_value();
			auto guard = make_manual_scope_guard([&](){
				new (std::addressof(this->value())) T(std::move(tmp));
			});
			new (std::addressof(this->error())) E(other.error());
			guard.active = false;
		}
		has_value() = false;
	}
};

} /* namespace detail */

using in_place_t = std::in_place_t;
inline constexpr in_place_t in_place = std::in_place;

struct in_place_error_t{};
inline constexpr in_place_error_t in_place_error = in_place_error_t{};


template <class T, class E>
struct Result;

template <class E>
struct Error;

namespace traits {

template <class R>
struct is_result: std::false_type {};

template <class T, class E>
struct is_result<Result<T, E>>: std::true_type {};

template <class R>
inline constexpr bool is_result_v = is_result<R>::value;

template <class R>
struct is_error: std::false_type {};

template <class E>
struct is_error<Error<E>>: std::true_type {};

template <class E>
inline constexpr bool is_error_v = is_error<E>::value;

namespace detail {

template <class T, decltype((std::declval<T>() ? true : false)) = false>
constexpr std::true_type is_contextually_convertible_to_bool_helper(std::in_place_type_t<T>, int) {
	return std::true_type{};
}

template <class T>
constexpr std::false_type is_contextually_convertible_to_bool_helper(std::in_place_type_t<T>, ...) {
	return std::false_type{};
}

template <class T>
struct is_contextually_convertible_to_bool:
	decltype(tim::traits::detail::is_contextually_convertible_to_bool_helper(std::in_place_type<T>, 0))
{
};

template <class T>
inline constexpr bool is_contextually_convertible_to_bool_v
	= is_contextually_convertible_to_bool<T>::value;


static_assert(is_contextually_convertible_to_bool_v<bool>);
static_assert(is_contextually_convertible_to_bool_v<int>);
static_assert(is_contextually_convertible_to_bool_v<void*>);
struct NotConvertibleToBool {};
static_assert(!is_contextually_convertible_to_bool_v<NotConvertibleToBool>);

} /* namespace detail */

} /* namespace traits */



template <class E>
struct Error {
	static_assert(std::is_object_v<E>,
		"Instantiating Error<E> for non object type 'E' is not permitted.");
	static_assert(!std::is_array_v<E>,
		"Instantiating Error<E> for array type 'E' is not permitted.");
	static_assert(!traits::is_error_v<E>,
		"Instantiating Error<Error<E>> is not permitted.");
	static_assert(std::is_same_v<E, std::remove_cv_t<E>>,
		"Instantiating Error<E> for const- or volatile-qualified 'E' is not permitted.");
	using value_type = E;
	Error() = delete;
	Error(const Error&) = default;
	Error(Error&&) = default;

	template <
		class Err,
		std::enable_if_t<
			std::is_constructible_v<E, Err>
			&& !std::is_same_v<std::decay_t<Err>, in_place_t>
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
	constexpr explicit Error(in_place_t, Args&& ... args) noexcept(
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
	constexpr explicit Error(in_place_t, std::initializer_list<U> ilist, Args&& ... args) noexcept(
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
explicit Error(E&&) -> Error<std::decay_t<E>>;

template <
	class E,
	std::enable_if_t<
		std::is_constructible_v<
			Error<std::decay_t<E>>,
			E&&
		>,
		bool
	> = false
>
constexpr Error<std::decay_t<E>> make_error(E&& err)
	noexcept(std::is_nothrow_constructible_v<Error<std::decay_t<E>>, E&&>)
{
	return Error<std::decay_t<E>>(std::forward<E>(err));
}

template <class E>
constexpr void swap(Error<E>& lhs, Error<E>& rhs)
	noexcept(noexcept(lhs.swap(rhs)))
{
	lhs.swap(rhs);
}

template <class E1, class E2, class = decltype(std::declval<E1>() == std::declval<E2>())>
constexpr bool operator==(const Error<E1>& lhs, const Error<E2>& rhs)
	noexcept(noexcept(lhs.value() == rhs.value()))
{
	return lhs.value() == rhs.value();
}

template <class E1, class E2, class = decltype(std::declval<E1>() != std::declval<E2>())>
constexpr bool operator!=(const Error<E1>& lhs, const Error<E2>& rhs)
	noexcept(noexcept(lhs.value() != rhs.value()))
{
	return lhs.value() != rhs.value();
}

namespace detail {

template <class E>
struct is_error_type: std::false_type { };

template <class E>
struct is_error_type<Error<E>>: std::true_type { };

template <class E>
struct error_value_type {};

template <class E>
struct error_value_type<Error<E>> {
	using type = E;
};

template <class E>
using error_value_type_t = typename error_value_type<E>::type;

} /* namespace detail */

template <class E>
class BadResultAccess;

template <>
class BadResultAccess<void>: std::exception {
public:
	BadResultAccess() = default;

	virtual const char* what() const noexcept override {
		return "bad result access";
	}

};

template <class E, bool = std::is_copy_constructible_v<E>, bool = std::is_move_constructible_v<E>>
class BadResultAccessBase;

template <class E>
class BadResultAccessBase<E, false, false>: BadResultAccess<void> {
public:
	BadResultAccessBase() = default;
};

template <class E>
class BadResultAccessBase<E, true, false>: BadResultAccess<void> {
public:
	BadResultAccessBase() = default;

	explicit constexpr BadResultAccessBase(E&& err) = delete;

	explicit constexpr BadResultAccessBase(const E& err):
		BadResultAccess<void>(),
		error_(err)
	{
		
	}

	constexpr E& error() & {
		return error_;
	}

	constexpr const E& error() const& {
		return error_;
	}

	constexpr E&& error() && {
		return std::move(error_);
	}

	constexpr const E&& error() const&& {
		return std::move(error_);
	}

private:
	E error_;
};

template <class E>
class BadResultAccessBase<E, false, true>: BadResultAccess<void> {
public:
	BadResultAccessBase() = default;

	explicit constexpr BadResultAccessBase(const E& err) = delete;

	explicit constexpr BadResultAccessBase(E&& err):
		BadResultAccess<void>(),
		error_(std::move(err))
	{
		
	}

	constexpr E& error() & {
		return error_;
	}

	constexpr const E& error() const& {
		return error_;
	}

	constexpr E&& error() && {
		return std::move(error_);
	}

	constexpr const E&& error() const&& {
		return std::move(error_);
	}

private:
	E error_;
};

template <class E>
class BadResultAccessBase<E, true, true>: BadResultAccess<void> {
public:
	BadResultAccessBase() = delete;

	explicit constexpr BadResultAccessBase(const E& err):
		BadResultAccess<void>(),
		error_(err)
	{
		
	}

	explicit constexpr BadResultAccessBase(E&& err):
		BadResultAccess<void>(),
		error_(std::move(err))
	{
		
	}

	constexpr E& error() & {
		return error_;
	}

	constexpr const E& error() const& {
		return error_;
	}

	constexpr E&& error() && {
		return std::move(error_);
	}

	constexpr const E&& error() const&& {
		return std::move(error_);
	}

private:
	E error_;
};

template <class E>
class BadResultAccess: BadResultAccessBase<E> {
	using base_type = BadResultAccessBase<E>;
	using base_type::base_type;

	BadResultAccess() = default;

};

template <class T, class E>
struct Result {
	static_assert(std::is_object_v<E>,
		"Instantiating Result<T, E> for non object type 'E' is not permitted.");
	static_assert(!std::is_array_v<E>,
		"Instantiating Result<T, E> for array type 'E' is not permitted.");
	static_assert(!traits::is_error_v<E>,
		"Instantiating Result<T, Error<E>> is not permitted.");
	static_assert(std::is_same_v<E, std::remove_cv_t<E>>,
		"Instantiating Result<T, E> for const- or volatile-qualified 'E' is not permitted.");
	static_assert(!std::is_reference_v<T>,
		"Instantiating Result<T, E> for reference type 'T' is not permitted.");
	static_assert(!std::is_function_v<T>,
		"Instantiating Result<T, E> for function type 'T' is not permitted.");
	static_assert(!std::is_same_v<tim::result::in_place_t, std::remove_cv_t<T>>,
		"Instantiating Result<T, E> where 'T' is const- or volatile-qualified 'in_place_t' is not permitted.");
	static_assert(!std::is_same_v<tim::result::in_place_error_t, std::remove_cv_t<T>>,
		"Instantiating Result<T, E> where 'T' is const- or volatile-qualified 'in_place_error_t' is not permitted.");
	static_assert(!traits::is_error_v<T>,
		"Instantiating Result<Error<T>, E> is not permitted.");


	template <class U, class G>
	friend struct Result;
private:
	using data_type = detail::result_data_type<T, E>;
	static constexpr detail::value_tag_t value_tag = detail::value_tag;
	static constexpr detail::error_tag_t error_tag = detail::error_tag;
public:

	using value_type = T;
	using error_type = E;

	static_assert(
		!detail::is_error_type<T>::value,
		"Result<Error<T>, E> is not permitted."
	);

	Result() = default;
	Result(const Result&) = default;
	Result(Result&&) = default;

	template <
		class U,
		class G,
		std::enable_if_t<
			!std::conjunction_v<
				std::is_convertible<const U&, T>,
				std::is_convertible<const G&, E>
			>
			&& std::conjunction_v<
				std::is_constructible<T, const U&>,
				std::negation<std::is_constructible<T, Result<U, G>&>>,
				std::negation<std::is_constructible<T, Result<U, G>&&>>,
				std::negation<std::is_constructible<T, const Result<U, G>&>>,
				std::negation<std::is_constructible<T, const Result<U, G>&&>>,
				std::negation<std::is_convertible<Result<U, G>&, T>>,
				std::negation<std::is_convertible<Result<U, G>&&, T>>,
				std::negation<std::is_convertible<const Result<U, G>&, T>>,
				std::negation<std::is_convertible<const Result<U, G>&&, T>>,
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
				return data_type(value_tag, other.val());
			} else {
				return data_type(error_tag, other.err());
			}
		}())
	{
		
	}

	template <
		class U,
		class G,
		std::enable_if_t<
			std::conjunction_v<
				std::is_convertible<const U&, T>,
				std::is_convertible<const G&, E>
			>
			&& std::conjunction_v<
				std::is_constructible<T, const U&>,
				std::negation<std::is_constructible<T, Result<U, G>&>>,
				std::negation<std::is_constructible<T, Result<U, G>&&>>,
				std::negation<std::is_constructible<T, const Result<U, G>&>>,
				std::negation<std::is_constructible<T, const Result<U, G>&&>>,
				std::negation<std::is_convertible<Result<U, G>&, T>>,
				std::negation<std::is_convertible<Result<U, G>&&, T>>,
				std::negation<std::is_convertible<const Result<U, G>&, T>>,
				std::negation<std::is_convertible<const Result<U, G>&&, T>>,
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
				return data_type(value_tag, other.val());
			} else {
				return data_type(error_tag, other.err());
			}
		}())
	{
		
	}

	template <
		class U,
		class G,
		std::enable_if_t<
			!std::conjunction_v<
				std::is_convertible<U&&, T>,
				std::is_convertible<G&&, E>
			>
			&& std::conjunction_v<
				std::is_constructible<T, U&&>,
				std::negation<std::is_constructible<T, Result<U, G>&>>,
				std::negation<std::is_constructible<T, Result<U, G>&&>>,
				std::negation<std::is_constructible<T, const Result<U, G>&>>,
				std::negation<std::is_constructible<T, const Result<U, G>&&>>,
				std::negation<std::is_convertible<Result<U, G>&, T>>,
				std::negation<std::is_convertible<Result<U, G>&&, T>>,
				std::negation<std::is_convertible<const Result<U, G>&, T>>,
				std::negation<std::is_convertible<const Result<U, G>&&, T>>,
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
	constexpr explicit Result(Result<U, G>&& other) noexcept(
		std::is_nothrow_constructible_v<T, U&&>
		&& std::is_nothrow_constructible_v<E, G&&>
	):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(value_tag, std::move(other.val()));
			} else {
				return data_type(error_tag, std::move(other.err()));
			}
		}())
	{
		
	}
	
	template <
		class U,
		class G,
		std::enable_if_t<
			std::conjunction_v<
				std::is_convertible<U&&, T>,
				std::is_convertible<G&&, E>
			>
			&& std::conjunction_v<
				std::is_constructible<T, U&&>,
				std::negation<std::is_constructible<T, Result<U, G>&>>,
				std::negation<std::is_constructible<T, Result<U, G>&&>>,
				std::negation<std::is_constructible<T, const Result<U, G>&>>,
				std::negation<std::is_constructible<T, const Result<U, G>&&>>,
				std::negation<std::is_convertible<Result<U, G>&, T>>,
				std::negation<std::is_convertible<Result<U, G>&&, T>>,
				std::negation<std::is_convertible<const Result<U, G>&, T>>,
				std::negation<std::is_convertible<const Result<U, G>&&, T>>,
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
	constexpr Result(Result<U, G>&& other) noexcept(
		std::is_nothrow_constructible_v<T, U&&>
		&& std::is_nothrow_constructible_v<E, G&&>
	):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(value_tag, std::move(other.val()));
			} else {
				return data_type(error_tag, std::move(other.err()));
			}
		}())
	{
		
	}
	
	template <
		class U = T,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<std::is_convertible<U&&, T>>,
				std::is_constructible<T, U&&>,
				std::negation<std::is_same<std::decay_t<U>, in_place_t>>,
				std::negation<std::is_same<std::decay_t<U>, Result>>,
				std::negation<std::is_same<std::decay_t<U>, Error<E>>>
			>,
			bool
		> = false
	>
	constexpr explicit Result(U&& v) noexcept(std::is_nothrow_constructible_v<T, U&&>):
		data_(value_tag, std::forward<U>(v))
	{
			
	}

	template <
		class U = T,
		std::enable_if_t<
			std::conjunction_v<
				std::is_convertible<U&&, T>,
				std::is_constructible<T, U&&>,
				std::negation<std::is_same<std::decay_t<U>, in_place_t>>,
				std::negation<std::is_same<std::decay_t<U>, Result>>,
				std::negation<std::is_same<std::decay_t<U>, Error<E>>>
			>,
			bool
		> = false
	>
	constexpr Result(U&& v) noexcept(std::is_nothrow_constructible_v<T, U&&>):
		data_(value_tag, std::forward<U>(v))
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::is_convertible<const G&, E>,
				std::is_constructible<E, const G&>
			>,
			bool
		> = false
	>
	constexpr Result(const Error<G>& v) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_(error_tag, v.value())
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<std::is_convertible<const G&, E>>,
				std::is_constructible<E, const G&>
			>,
			bool
		> = false
	>
	constexpr explicit Result(const Error<G>& v) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_(error_tag, v.value())
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::is_convertible<G&&, E>,
				std::is_constructible<E, G&&>
			>,
			bool
		> = false
	>
	constexpr Result(Error<G>&& v) noexcept(std::is_nothrow_constructible_v<E, G&>):
		data_(error_tag, std::move(v.value()))
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<std::is_convertible<G&&, E>>,
				std::is_constructible<E, G&&>
			>,
			bool
		> = false
	>
	constexpr explicit Result(Error<G>&& v) noexcept(std::is_nothrow_constructible_v<E, G&&>):
		data_(error_tag, std::move(v.value()))
	{
			
	}

	template <
		class ... Args,
		std::enable_if_t<
			std::is_constructible_v<T, Args...>,
			bool
		> = false
	>
	constexpr explicit Result(in_place_t, Args&& ... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>):
		data_(value_tag, std::forward<Args>(args)...)
	{
			
	}

	template <
		class U,
		class ... Args,
		std::enable_if_t<
			std::is_constructible_v<T, std::initializer_list<U>&, Args&&...>,
			bool
		> = false
	>
	constexpr explicit Result(in_place_t, std::initializer_list<U> ilist, Args&& ... args) noexcept(
		std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args&&...>
	):
		data_(value_tag, ilist, std::forward<Args>(args)...)
	{
			
	}

	template <
		class ... Args,
		std::enable_if_t<std::is_constructible_v<E, Args&&...>, bool> = false
	>
	constexpr explicit Result(in_place_error_t, Args&& ... args) noexcept(std::is_nothrow_constructible_v<E, Args&&...>):
		data_(error_tag, std::forward<Args>(args)...)
	{
			
	}

	template <
		class U,
		class ... Args,
		std::enable_if_t<
			std::is_constructible_v<E, std::initializer_list<U>&, Args&&...>,
			bool
		> = false
	>
	constexpr explicit Result(in_place_error_t, std::initializer_list<U> ilist, Args&& ... args) noexcept(
		std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args&& ...>
	):
		data_(error_tag, ilist, std::forward<Args>(args)...)
	{
			
	}

	constexpr Result& operator=(const Result&) = default;
	constexpr Result& operator=(Result&&) = default;

	template <
		class U,
		std::enable_if_t<
			std::conjunction_v<
    				std::negation<std::is_same<Result, std::decay_t<U>>>,
    				std::negation<std::conjunction<
					std::is_scalar<T>,
					std::is_same<T, std::decay_t<U>>
				>>,
    				std::is_constructible<T, U&&>,
    				std::is_assignable<T&, U&&>,
				std::disjunction<
					std::is_nothrow_constructible<T, U&&>,
					std::is_nothrow_move_constructible<E>
				>
			>,
			bool
		> = false
	>
	constexpr Result& operator=(U&& v) noexcept(
		std::is_nothrow_assignable_v<T&, U&&>
		&& std::is_nothrow_constructible_v<T, U&&>
	)
	{
		if(this->has_value()) {
			this->val() = std::forward<U>(v);
			return *this;
		}
		if constexpr(std::is_nothrow_constructible_v<T, U&&>) {
			this->destruct_error();
			new (std::addressof(this->val())) T(std::forward<U>(v));
		} else {
			static_assert(std::is_nothrow_move_constructible_v<E>);
			E tmp(std::move(this->err()));
			this->destruct_error();
			auto guard = detail::make_manual_scope_guard([&](){
				new (std::addressof(this->err())) E(std::move(tmp));
			});
			new (std::addressof(this->val())) T(std::forward<U>(v));
			guard.active = false;
		}
		data_.has_value() = true;
		return *this;
	}

	template <
		class G,
		std::enable_if_t<
			std::conjunction_v<
    				std::is_constructible<E, const G&>,
    				std::is_assignable<E&, const G&>,
				std::disjunction<
					std::is_nothrow_constructible<E, const G&>,
					std::is_nothrow_move_constructible<T>
				>
			>,
			bool
		> = false
	>
	constexpr Result& operator=(const Error<G>& e) noexcept(
		std::is_nothrow_assignable_v<E&, const G&>
		&& std::is_nothrow_constructible_v<E, const G&>
	)
	{
		if(!this->has_value()) {
			this->err() = e.value();
			return *this;
		}
		if constexpr(std::is_nothrow_constructible_v<E, const G&>) {
			this->destruct_value();
			new (std::addressof(this->err())) E(e.value());
		} else {
			static_assert(std::is_nothrow_move_constructible_v<T>);
			T tmp(std::move(this->val()));
			this->destruct_error();
			auto guard = detail::make_manual_scope_guard([&](){
				new (std::addressof(this->val())) T(std::move(tmp));
			});
			new (std::addressof(this->err())) E(e.value());
			guard.active = false;
		}
		data_.has_value() = false;
		return *this;
	}

	template <
		class G,
		std::enable_if_t<
			std::conjunction_v<
    				std::is_constructible<E, G&&>,
    				std::is_assignable<E&, G&&>,
				std::disjunction<
					std::is_nothrow_constructible<E, G&&>,
					std::is_nothrow_move_constructible<T>
				>
			>,
			bool
		> = false
	>
	constexpr Result& operator=(Error<G>&& e) noexcept(
		std::is_nothrow_assignable_v<E&, G&&>
		&& std::is_nothrow_constructible_v<E, G&&>
	) {
		if(!this->has_value()) {
			this->err() = std::move(e.value());
			return *this;
		}
		if constexpr(std::is_nothrow_constructible_v<E, G&&>) {
			this->destruct_value();
			new (std::addressof(this->err())) E(std::move(e.value()));
		} else {
			static_assert(std::is_nothrow_move_constructible_v<T>);
			T tmp(std::move(this->val()));
			this->destruct_error();
			auto guard = detail::make_manual_scope_guard([&](){
				new (std::addressof(this->val())) T(std::move(tmp));
			});
			new (std::addressof(this->err())) E(std::move(e.value()));
			guard.active = false;
		}
		data_.has_value() = false;
		return *this;
	}

	template <
		class ... Args,
		std::enable_if_t<
			std::conjunction_v<
				std::is_constructible<T, Args&&...>,
				std::disjunction<
					std::is_nothrow_constructible<T, Args&&...>,
					std::is_nothrow_move_constructible<T>,
					std::conjunction<
						std::is_nothrow_move_constructible<E>,
						std::is_move_assignable<T>
					>
				>
			>,
			bool
		> = false
	>
	constexpr T& emplace(Args&& ... args) noexcept(
		std::is_nothrow_constructible_v<T, Args&&...>
	) {
		if constexpr(std::is_nothrow_constructible_v<T, Args&& ...>) {
			if(this->has_value()) {
				this->destruct_value();
				new (std::addressof(this->val())) T(std::forward<Args>(args)...);
			} else {
				this->destruct_error();
				new (std::addressof(this->val())) T(std::forward<Args>(args)...);
				data_.has_value() = true;
			}
			return this->val();
		} else if constexpr(std::is_nothrow_move_constructible_v<T>) {
			T tmp(std::forward<Args>(args)...);
			if(this->has_value()) {
				this->destruct_value();
				new (std::addressof(this->val())) T(std::move(tmp));
			} else {
				this->destruct_error();
				new (std::addressof(this->val())) T(std::move(tmp));
				data_.has_value() = true;
			}
			return this->val();
		} else {
			static_assert(std::is_nothrow_move_constructible_v<E> && std::is_move_assignable_v<T>);
			if(this->has_value()) {
				this->val() = T(std::forward<Args>(args)...);
				return this->val();
			} else {
				E tmp(std::move(this->err()));
				this->destruct_error();
				auto guard = detail::make_manual_scope_guard([&](){
					new (std::addressof(this->err())) E(std::move(tmp));
				});
				new (std::addressof(this->val())) T(std::forward<Args>(args)...);
				guard.active = false;
				data_.has_value() = true;
			}
		}
	}

	template <
		class U,
		class ... Args,
		std::enable_if_t<
			std::conjunction_v<
				std::is_constructible<T, std::initializer_list<U>&, Args&&...>,
				std::disjunction<
					std::is_nothrow_constructible<T, std::initializer_list<U>&, Args&&...>,
					std::is_nothrow_move_constructible<T>,
					std::conjunction<
						std::is_nothrow_move_constructible<E>,
						std::is_move_assignable<T>
					>
				>
			>,
			bool
		> = false
	>
	constexpr T& emplace(std::initializer_list<U> ilist, Args&& ... args) noexcept(
		std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args&&...>
	) {
		if constexpr(std::is_nothrow_constructible_v<T, Args&& ...>) {
			if(this->has_value()) {
				this->destruct_value();
				new (std::addressof(this->val())) T(ilist, std::forward<Args>(args)...);
			} else {
				this->destruct_error();
				new (std::addressof(this->val())) T(ilist, std::forward<Args>(args)...);
				data_.has_value() = true;
			}
			return this->val();
		} else if constexpr(std::is_nothrow_move_constructible_v<T>) {
			T tmp(ilist, std::forward<Args>(args)...);
			if(this->has_value()) {
				this->destruct_value();
				new (std::addressof(this->val())) T(std::move(tmp));
			} else {
				this->destruct_error();
				new (std::addressof(this->val())) T(std::move(tmp));
				data_.has_value() = true;
			}
			return this->val();
		} else {
			static_assert(std::is_nothrow_move_constructible_v<E> && std::is_move_assignable_v<T>);
			if(this->has_value()) {
				this->val() = T(ilist, std::forward<Args>(args)...);
				return this->val();
			} else {
				E tmp(std::move(this->err()));
				this->destruct_error();
				auto guard = detail::make_manual_scope_guard([&](){
					new (std::addressof(this->err())) E(std::move(tmp));
				});
				new (std::addressof(this->val())) T(ilist, std::forward<Args>(args)...);
				guard.active = false;
				data_.has_value() = true;
			}
		}
	}

	template <
		class Other = Result,
		std::enable_if_t<
			std::conjunction_v<
				std::is_same<Other, Result>,
				std::is_move_constructible<T>,
				std::is_swappable<T>,
				std::is_move_constructible<E>,
				std::is_swappable<E>
			>,
			bool
		> = false
	>
	constexpr void swap(Other& other) noexcept(
		std::conjunction_v<
			std::is_nothrow_move_constructible<T>,
			std::is_nothrow_move_constructible<E>,
			std::is_nothrow_swappable<T>,
			std::is_nothrow_swappable<E>
		>
	) {
		if(this->has_value()) {
			if(other.has_value()) {
				this->swap_case(other, std::true_type{}, std::true_type{});
			} else {
				this->swap_case(other, std::true_type{}, std::false_type{});
			}
		} else {
			if(other.has_value()) {
				this->swap_case(other, std::false_type{}, std::true_type{});
				
			} else {
				this->swap_case(other, std::false_type{}, std::false_type{});
			}
		}
	}

	constexpr bool has_value() const {
		return this->data_.has_value();
	}

	explicit constexpr operator bool() const {
		return this->has_value();
	}

	constexpr const T* operator->() const {
		assert_has_value();
		return std::addressof(this->val());
	}

	constexpr T* operator->() {
		assert_has_value();
		return std::addressof(this->val());
	}

	constexpr const T&& operator*() const&& {
		assert_has_value();
		return std::move(this->val());
	}

	constexpr const T& operator*() const& {
		assert_has_value();
		return this->val();
	}

	constexpr T&& operator*() && {
		assert_has_value();
		return std::move(this->val());
	}

	constexpr T& operator*() & {
		assert_has_value();
		return this->val();
	}

	constexpr const T& value() const& {
		if(!this->has_value()) {
			this->throw_bad_result_access();
		}
		return this->val();
	}

	constexpr const T&& value() const&& {
		if(!this->has_value()) {
			std::move(*this).throw_bad_result_access();
		}
		return std::move(this->val());
	}

	constexpr T& value() & {
		if(!this->has_value()) {
			this->throw_bad_result_access();
		}
		return this->val();
	}

	constexpr T&& value() && {
		if(!this->has_value()) {
			std::move(*this).throw_bad_result_access();
		}
		return std::move(this->val());
	}

	constexpr E&& error() && {
		assert_not_has_value();
		return std::move(this->err());
	}

	constexpr const E&& error() const&& {
		assert_not_has_value();
		return std::move(this->err());
	}

	constexpr E& error() & {
		assert_not_has_value();
		return this->err();
	}

	constexpr const E& error() const& {
		assert_not_has_value();
		return this->err();
	}

	template <class U>
	constexpr T value_or(U&& alt) const& {
		if(this->has_value()) {
			return this->val();
		}
		return std::forward<U>(alt);
	}

	template <class U>
	constexpr T value_or(U&& alt) && {
		if(this->has_value()) {
			return std::move(this->val());
		}
		return std::forward<U>(alt);
	}

private:

	constexpr void assert_has_value() const {
#if defined(assert) && !defined(TIM_RESULT_DISABLE_ASSERTIONS)
		assert(this->has_value());
#endif
	}

	constexpr void assert_not_has_value() const {
#if defined(assert) && defined(TIM_RESULT_DISABLE_ASSERTIONS)
		assert(not this->has_value());
#endif
	}

	[[noreturn]]
	void throw_bad_result_access() const& noexcept(false) {
		if constexpr(std::is_move_constructible_v<E>) {
			throw BadResultAccess<E>(this->error());
		} else {
			throw BadResultAccess<E>();
		}
	}
	
	[[noreturn]]
	void throw_bad_result_access() && noexcept(false) {
		if constexpr(std::is_move_constructible_v<E>) {
			throw BadResultAccess<E>(std::move(this->error()));
		} else {
			throw BadResultAccess<E>();
		}
	}
	
	constexpr void swap_case(Result& other, std::false_type, std::false_type) {
		using std::swap;
		swap(this->err(), other.err());
	}

	constexpr void swap_case(Result& other, std::false_type, std::true_type) {
		other.swap_case(*this, std::true_type{}, std::false_type{});
	}

	constexpr void swap_case(Result& other, std::true_type, std::false_type) {
		if constexpr(std::is_nothrow_move_constructible_v<E>) {
			E tmp(std::move(other.err()));
			other.destruct_error();
			auto guard = detail::make_manual_scope_guard([&](){
				new (std::addressof(other.err())) E(std::move(tmp));
			});
			new (std::addressof(other.val())) T(std::move(this->val()));
			guard.active = false;
			other.data_.has_value() = true;
			this->destruct_value();
			new (std::addressof(this->err())) E(std::move(tmp));
			this->data_.has_value() = false;
		} else {
			static_assert(std::is_nothrow_move_constructible_v<T>);
			T tmp(std::move(this->val()));
			this->destruct_value();
			auto guard = detail::make_manual_scope_guard([&](){
				new (std::addressof(this->val())) T(std::move(tmp));
			});
			new (std::addressof(this->err())) E(std::move(other.err()));
			guard.active = false;
			this->data_.has_value() = false;
			other.destruct_error();
			new (std::addressof(other.val())) T(std::move(tmp));
			other.data_.has_value() = true;
		}
	}
	
	constexpr void swap_case(Result& other, std::true_type, std::true_type) {
		using std::swap;
		swap(this->val(), other.val());
	}
	
	constexpr void destruct_value() noexcept {
		return data_.destruct_value();
	}

	constexpr void destruct_error() noexcept {
		return data_.destruct_error();
	}

	constexpr void destruct() noexcept {
		return data_.destruct();
	}

	constexpr const T& val() const {
		return this->data_.value();
	}

	constexpr T& val() {
		return this->data_.value();
	}

	constexpr const E& err() const {
		return this->data_.error();
	}

	constexpr E& err() {
		return this->data_.error();
	}

	data_type data_;
};

template <class E>
struct Result<void, E> {
private:
	template <class U, class G>
	friend struct Result;

	using data_type = detail::result_data_type<void, E>;
	static constexpr detail::value_tag_t value_tag = detail::value_tag;
	static constexpr detail::error_tag_t error_tag = detail::error_tag;
public:

	using value_type = void;
	using error_type = E;

	Result() = default;
	Result(const Result&) = default;
	Result(Result&&) = default;

	template <
		class G,
		class U,
		std::enable_if_t<
			!std::is_convertible_v<const G&, E>
			&& std::conjunction_v<
				detail::is_cv_void<U>,
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
	explicit constexpr Result(const Result<U, G>& other) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(value_tag);
			} else {
				return data_type(error_tag, other.err());
			}
		}())
	{
		
	}

	template <
		class U,
		class G,
		std::enable_if_t<
			std::is_convertible_v<const G&, E>
			&& std::conjunction_v<
				detail::is_cv_void<U>,
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
	constexpr Result(const Result<U, G>& other) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(value_tag);
			} else {
				return data_type(error_tag, other.err());
			}
		}())
	{
		
	}

	template <
		class U,
		class G,
		std::enable_if_t<
			!std::is_convertible_v<G&&, E>
			&& std::conjunction_v<
				detail::is_cv_void<U>,
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
	constexpr explicit Result(Result<U, G>&& other) noexcept(std::is_nothrow_constructible_v<E, G&&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(value_tag);
			} else {
				return data_type(error_tag, std::move(other.err()));
			}
		}())
	{
		
	}
	
	template <
		class U,
		class G,
		std::enable_if_t<
			std::is_convertible_v<G&&, E>
			&& std::conjunction_v<
				detail::is_cv_void<U>,
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
	constexpr Result(Result<U, G>&& other) noexcept(std::is_nothrow_constructible_v<E, G&&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(value_tag);
			} else {
				return data_type(error_tag, std::move(other.err()));
			}
		}())
	{
		
	}
	
	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::is_convertible<const G&, E>,
				std::is_constructible<E, const G&>
			>,
			bool
		> = false
	>
	constexpr Result(const Error<G>& v) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_(error_tag, v.value())
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<std::is_convertible<const G&, E>>,
				std::is_constructible<E, const G&>
			>,
			bool
		> = false
	>
	constexpr explicit Result(const Error<G>& v) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_(error_tag, v.value())
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::is_convertible<G&&, E>,
				std::is_constructible<E, G&&>
			>,
			bool
		> = false
	>
	constexpr Result(Error<G>&& v) noexcept(std::is_nothrow_constructible_v<E, G&>):
		data_(error_tag, std::move(v.value()))
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<std::is_convertible<G&&, E>>,
				std::is_constructible<E, G&&>
			>,
			bool
		> = false
	>
	constexpr explicit Result(Error<G>&& v) noexcept(std::is_nothrow_constructible_v<E, G&&>):
		data_(error_tag, std::move(v.value()))
	{
			
	}

	constexpr explicit Result(in_place_t) noexcept:
		data_(value_tag)
	{
			
	}

	template <
		class ... Args,
		std::enable_if_t<std::is_constructible_v<E, Args&&...>, bool> = false
	>
	constexpr explicit Result(in_place_error_t, Args&& ... args) noexcept(std::is_nothrow_constructible_v<E, Args&&...>):
		data_(error_tag, std::forward<Args>(args)...)
	{
			
	}

	template <
		class U,
		class ... Args,
		std::enable_if_t<
			std::is_constructible_v<E, std::initializer_list<U>&, Args&&...>,
			bool
		> = false
	>
	constexpr explicit Result(in_place_error_t, std::initializer_list<U> ilist, Args&& ... args) noexcept(
		std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args&&...>
	):
		data_(error_tag, ilist, std::forward<Args>(args)...)
	{
			
	}

	constexpr Result& operator=(const Result&) = default;
	constexpr Result& operator=(Result&&) = default;

	template <
		class G,
		std::enable_if_t<
			std::conjunction_v<
    				std::is_constructible<E, const G&>,
    				std::is_assignable<E&, const G&>
			>,
			bool
		> = false
	>
	constexpr Result& operator=(const Error<G>& e) noexcept(
		std::is_nothrow_assignable_v<E&, const G&>
		&& std::is_nothrow_constructible_v<E, const G&>
	)
	{
		if(!this->has_value()) {
			this->err() = e.value();
			return *this;
		}
		new (std::addressof(this->err())) E(e.value());
		data_.has_value() = false;
		return *this;
	}

	template <
		class G,
		std::enable_if_t<
			std::conjunction_v<
    				std::is_constructible<E, G&&>,
    				std::is_assignable<E&, G&&>
			>,
			bool
		> = false
	>
	constexpr Result& operator=(Error<G>&& e) noexcept(
		std::is_nothrow_assignable_v<E&, G&&>
		&& std::is_nothrow_constructible_v<E, G&&>
	) {
		if(!this->has_value()) {
			this->err() = std::move(e.value());
			return *this;
		}
		new (std::addressof(this->err())) E(std::move(e.value()));
		data_.has_value() = false;
		return *this;
	}

	constexpr void emplace() noexcept {
		if(this->has_value()) {
			return;
		}
		this->destruct_error();
		data_.has_value() = true;
	}

	template <
		class Other = Result,
		std::enable_if_t<
			std::conjunction_v<
				std::is_same<Other, Result>,
				std::is_move_constructible<E>,
				std::is_swappable<E>
			>,
			bool
		> = false
	>
	constexpr void swap(Other& other) noexcept(
		std::conjunction_v<
			std::is_nothrow_move_constructible<E>,
			std::is_nothrow_swappable<E>
		>
	) {
		if(this->has_value()) {
			if(other.has_value()) {
				return;
			} else {
				new (std::addressof(this->error())) E(std::move(other.error()));
				this->data_.has_value() = false;
				other.destruct_error();
				other.data_.has_value() = false;
			}
		} else {
			if(other.has_value()) {
				new (std::addressof(other.error())) E(std::move(this->error()));
				other.data_.has_value() = false;
				this->destruct_error();
				this->data_.has_value() = true;
				
			} else {
				using std::swap;
				swap(this->error(), other.error());
			}
		}
	}

	constexpr bool has_value() const {
		return this->data_.has_value();
	}

	explicit constexpr operator bool() const {
		return this->has_value();
	}

	constexpr void value() const& {
		if(!this->has_value()) {
			this->throw_bad_result_access();
		}
	}

	constexpr void value() && {
		if(!this->has_value()) {
			std::move(*this).throw_bad_result_access();
		}
	}

	constexpr E&& error() && {
		assert_not_has_value();
		return std::move(this->err());
	}

	constexpr const E&& error() const&& {
		assert_not_has_value();
		return std::move(this->err());
	}

	constexpr E& error() & {
		assert_not_has_value();
		return this->err();
	}

	constexpr const E& error() const& {
		assert_not_has_value();
		return this->err();
	}

private:
	constexpr void assert_has_value() const {
#if defined(assert) && !defined(TIM_RESULT_DISABLE_ASSERTIONS)
		assert(this->has_value());
#endif
	}

	constexpr void assert_not_has_value() const {
#if defined(assert) && defined(TIM_RESULT_DISABLE_ASSERTIONS)
		assert(not this->has_value());
#endif
	}

	[[noreturn]]
	void throw_bad_result_access() const& noexcept(false) {
		if constexpr(std::is_move_constructible_v<E>) {
			throw BadResultAccess<E>(this->error());
		} else {
			throw BadResultAccess<E>();
		}
	}
	
	[[noreturn]]
	void throw_bad_result_access() && noexcept(false) {
		if constexpr(std::is_move_constructible_v<E>) {
			throw BadResultAccess<E>(std::move(this->error()));
		} else {
			throw BadResultAccess<E>();
		}
	}
	
	constexpr void destruct_value() noexcept {
		return data_.destruct_value();
	}

	constexpr void destruct_error() noexcept {
		return data_.destruct_error();
	}

	constexpr void destruct() noexcept {
		return data_.destruct();
	}

	constexpr const E& err() const {
		return this->data_.error();
	}

	constexpr E& err() {
		return this->data_.error();
	}

	data_type data_;
};

template <class E>
struct Result<const void, E> {
private:
	template <class U, class G>
	friend struct Result;

	using data_type = detail::result_data_type<const void, E>;
	static constexpr detail::value_tag_t value_tag = detail::value_tag;
	static constexpr detail::error_tag_t error_tag = detail::error_tag;
public:

	using value_type = const void;
	using error_type = E;

	Result() = default;
	Result(const Result&) = default;
	Result(Result&&) = default;

	template <
		class G,
		class U,
		std::enable_if_t<
			!std::is_convertible_v<const G&, E>
			&& std::conjunction_v<
				detail::is_cv_void<U>,
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
	explicit constexpr Result(const Result<U, G>& other) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(value_tag);
			} else {
				return data_type(error_tag, other.err());
			}
		}())
	{
		
	}

	template <
		class U,
		class G,
		std::enable_if_t<
			std::is_convertible_v<const G&, E>
			&& std::conjunction_v<
				detail::is_cv_void<U>,
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
	constexpr Result(const Result<U, G>& other) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(value_tag);
			} else {
				return data_type(error_tag, other.err());
			}
		}())
	{
		
	}

	template <
		class U,
		class G,
		std::enable_if_t<
			!std::is_convertible_v<G&&, E>
			&& std::conjunction_v<
				detail::is_cv_void<U>,
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
	constexpr explicit Result(Result<U, G>&& other) noexcept(std::is_nothrow_constructible_v<E, G&&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(value_tag);
			} else {
				return data_type(error_tag, std::move(other.err()));
			}
		}())
	{
		
	}
	
	template <
		class U,
		class G,
		std::enable_if_t<
			std::is_convertible_v<G&&, E>
			&& std::conjunction_v<
				detail::is_cv_void<U>,
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
	constexpr Result(Result<U, G>&& other) noexcept(std::is_nothrow_constructible_v<E, G&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(value_tag);
			} else {
				return data_type(error_tag, std::move(other.err()));
			}
		}())
	{
		
	}
	
	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::is_convertible<const G&, E>,
				std::is_constructible<E, const G&>
			>,
			bool
		> = false
	>
	constexpr Result(const Error<G>& v) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_(error_tag, v.value())
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<std::is_convertible<const G&, E>>,
				std::is_constructible<E, const G&>
			>,
			bool
		> = false
	>
	constexpr explicit Result(const Error<G>& v) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_(error_tag, v.value())
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::is_convertible<G&&, E>,
				std::is_constructible<E, G&&>
			>,
			bool
		> = false
	>
	constexpr Result(Error<G>&& v) noexcept(std::is_nothrow_constructible_v<E, G&>):
		data_(error_tag, std::move(v.value()))
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<std::is_convertible<G&&, E>>,
				std::is_constructible<E, G&&>
			>,
			bool
		> = false
	>
	constexpr explicit Result(Error<G>&& v) noexcept(std::is_nothrow_constructible_v<E, G&&>):
		data_(error_tag, std::move(v.value()))
	{
			
	}

	constexpr explicit Result(in_place_t) noexcept:
		data_(value_tag)
	{
			
	}

	template <
		class ... Args,
		std::enable_if_t<std::is_constructible_v<E, Args&&...>, bool> = false
	>
	constexpr explicit Result(in_place_error_t, Args&& ... args) noexcept(std::is_nothrow_constructible_v<E, Args&&...>):
		data_(error_tag, std::forward<Args>(args)...)
	{
			
	}

	template <
		class U,
		class ... Args,
		std::enable_if_t<
			std::is_constructible_v<E, std::initializer_list<U>&, Args&&...>,
			bool
		> = false
	>
	constexpr explicit Result(in_place_error_t, std::initializer_list<U> ilist, Args&& ... args) noexcept(
		std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args&&...>
	):
		data_(error_tag, ilist, std::forward<Args>(args)...)
	{
			
	}

	constexpr Result& operator=(const Result&) = default;
	constexpr Result& operator=(Result&&) = default;

	template <
		class G,
		std::enable_if_t<
			std::conjunction_v<
    				std::is_constructible<E, const G&>,
    				std::is_assignable<E&, const G&>
			>,
			bool
		> = false
	>
	constexpr Result& operator=(const Error<G>& e) noexcept(
		std::is_nothrow_assignable_v<E&, const G&>
		&& std::is_nothrow_constructible_v<E, const G&>
	)
	{
		if(!this->has_value()) {
			this->err() = e.value();
			return *this;
		}
		new (std::addressof(this->err())) E(e.value());
		data_.has_value() = false;
		return *this;
	}

	template <
		class G,
		std::enable_if_t<
			std::conjunction_v<
    				std::is_constructible<E, G&&>,
    				std::is_assignable<E&, G&&>
			>,
			bool
		> = false
	>
	constexpr Result& operator=(Error<G>&& e) noexcept(
		std::is_nothrow_assignable_v<E&, G&&>
		&& std::is_nothrow_constructible_v<E, G&&>
	) {
		if(!this->has_value()) {
			this->err() = std::move(e.value());
			return *this;
		}
		new (std::addressof(this->err())) E(std::move(e.value()));
		data_.has_value() = false;
		return *this;
	}

	constexpr void emplace() noexcept {
		if(this->has_value()) {
			return;
		}
		this->destruct_error();
		data_.has_value() = true;
	}

	template <
		class Other = Result,
		std::enable_if_t<
			std::conjunction_v<
				std::is_same<Other, Result>,
				std::is_move_constructible<E>,
				std::is_swappable<E>
			>,
			bool
		> = false
	>
	constexpr void swap(Other& other) noexcept(
		std::conjunction_v<
			std::is_nothrow_move_constructible<E>,
			std::is_nothrow_swappable<E>
		>
	) {
		if(this->has_value()) {
			if(other.has_value()) {
				return;
			} else {
				new (std::addressof(this->error())) E(std::move(other.error()));
				this->data_.has_value() = false;
				other.destruct_error();
				other.data_.has_value() = false;
			}
		} else {
			if(other.has_value()) {
				new (std::addressof(other.error())) E(std::move(this->error()));
				other.data_.has_value() = false;
				this->destruct_error();
				this->data_.has_value() = true;
				
			} else {
				using std::swap;
				swap(this->error(), other.error());
			}
		}
	}

	constexpr bool has_value() const {
		return this->data_.has_value();
	}

	explicit constexpr operator bool() const {
		return this->has_value();
	}

	constexpr void value() const& {
		if(!this->has_value()) {
			this->throw_bad_result_access();
		}
	}

	constexpr void value() && {
		if(!this->has_value()) {
			std::move(*this).throw_bad_result_access();
		}
	}

	constexpr E&& error() && {
		assert_not_has_value();
		return std::move(this->err());
	}

	constexpr const E&& error() const&& {
		assert_not_has_value();
		return std::move(this->err());
	}

	constexpr E& error() & {
		assert_not_has_value();
		return this->err();
	}

	constexpr const E& error() const& {
		assert_not_has_value();
		return this->err();
	}

private:
	constexpr void assert_has_value() const {
#if defined(assert) && !defined(TIM_RESULT_DISABLE_ASSERTIONS)
		assert(this->has_value());
#endif
	}

	constexpr void assert_not_has_value() const {
#if defined(assert) && defined(TIM_RESULT_DISABLE_ASSERTIONS)
		assert(not this->has_value());
#endif
	}

	[[noreturn]]
	void throw_bad_result_access() const& noexcept(false) {
		if constexpr(std::is_move_constructible_v<E>) {
			throw BadResultAccess<E>(this->error());
		} else {
			throw BadResultAccess<E>();
		}
	}
	
	[[noreturn]]
	void throw_bad_result_access() && noexcept(false) {
		if constexpr(std::is_move_constructible_v<E>) {
			throw BadResultAccess<E>(std::move(this->error()));
		} else {
			throw BadResultAccess<E>();
		}
	}
	
	constexpr void destruct_value() noexcept {
		return data_.destruct_value();
	}

	constexpr void destruct_error() noexcept {
		return data_.destruct_error();
	}

	constexpr void destruct() noexcept {
		return data_.destruct();
	}

	constexpr const E& err() const {
		return this->data_.error();
	}

	constexpr E& err() {
		return this->data_.error();
	}

	data_type data_;
};

template <class E>
struct Result<volatile void, E> {
private:
	template <class U, class G>
	friend struct Result;

	using data_type = detail::result_data_type<volatile void, E>;
	static constexpr detail::value_tag_t value_tag = detail::value_tag;
	static constexpr detail::error_tag_t error_tag = detail::error_tag;
public:

	using value_type = volatile void;
	using error_type = E;

	Result() = default;
	Result(const Result&) = default;
	Result(Result&&) = default;

	template <
		class G,
		class U,
		std::enable_if_t<
			!std::is_convertible_v<const G&, E>
			&& std::conjunction_v<
				detail::is_cv_void<U>,
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
	explicit constexpr Result(const Result<U, G>& other) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(value_tag);
			} else {
				return data_type(error_tag, other.err());
			}
		}())
	{
		
	}

	template <
		class U,
		class G,
		std::enable_if_t<
			std::is_convertible_v<const G&, E>
			&& std::conjunction_v<
				detail::is_cv_void<U>,
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
	constexpr Result(const Result<U, G>& other) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(value_tag);
			} else {
				return data_type(error_tag, other.err());
			}
		}())
	{
		
	}

	template <
		class U,
		class G,
		std::enable_if_t<
			!std::is_convertible_v<G&&, E>
			&& std::conjunction_v<
				detail::is_cv_void<U>,
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
	constexpr explicit Result(Result<U, G>&& other) noexcept(std::is_nothrow_constructible_v<E, G&&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(value_tag);
			} else {
				return data_type(error_tag, std::move(other.err()));
			}
		}())
	{
		
	}
	
	template <
		class U,
		class G,
		std::enable_if_t<
			std::is_convertible_v<G&&, E>
			&& std::conjunction_v<
				detail::is_cv_void<U>,
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
	constexpr Result(Result<U, G>&& other) noexcept(std::is_nothrow_constructible_v<E, G&&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(value_tag);
			} else {
				return data_type(error_tag, std::move(other.err()));
			}
		}())
	{
		
	}
	
	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::is_convertible<const G&, E>,
				std::is_constructible<E, const G&>
			>,
			bool
		> = false
	>
	constexpr Result(const Error<G>& v) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_(error_tag, v.value())
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<std::is_convertible<const G&, E>>,
				std::is_constructible<E, const G&>
			>,
			bool
		> = false
	>
	constexpr explicit Result(const Error<G>& v) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_(error_tag, v.value())
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::is_convertible<G&&, E>,
				std::is_constructible<E, G&&>
			>,
			bool
		> = false
	>
	constexpr Result(Error<G>&& v) noexcept(std::is_nothrow_constructible_v<E, G&>):
		data_(error_tag, std::move(v.value()))
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<std::is_convertible<G&&, E>>,
				std::is_constructible<E, G&&>
			>,
			bool
		> = false
	>
	constexpr explicit Result(Error<G>&& v) noexcept(std::is_nothrow_constructible_v<E, G&&>):
		data_(error_tag, std::move(v.value()))
	{
			
	}

	constexpr explicit Result(in_place_t) noexcept:
		data_(value_tag)
	{
			
	}

	template <
		class ... Args,
		std::enable_if_t<std::is_constructible_v<E, Args&&...>, bool> = false
	>
	constexpr explicit Result(in_place_error_t, Args&& ... args) noexcept(std::is_nothrow_constructible_v<E, Args&&...>):
		data_(error_tag, std::forward<Args>(args)...)
	{
			
	}

	template <
		class U,
		class ... Args,
		std::enable_if_t<
			std::is_constructible_v<E, std::initializer_list<U>&, Args&&...>,
			bool
		> = false
	>
	constexpr explicit Result(in_place_error_t, std::initializer_list<U> ilist, Args&& ... args) noexcept(
		std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args&&...>
	):
		data_(error_tag, ilist, std::forward<Args>(args)...)
	{
			
	}

	constexpr Result& operator=(const Result&) = default;
	constexpr Result& operator=(Result&&) = default;

	template <
		class G,
		std::enable_if_t<
			std::conjunction_v<
    				std::is_constructible<E, const G&>,
    				std::is_assignable<E&, const G&>
			>,
			bool
		> = false
	>
	constexpr Result& operator=(const Error<G>& e) noexcept(
		std::is_nothrow_assignable_v<E&, const G&>
		&& std::is_nothrow_constructible_v<E, const G&>
	)
	{
		if(!this->has_value()) {
			this->err() = e.value();
			return *this;
		}
		new (std::addressof(this->err())) E(e.value());
		data_.has_value() = false;
		return *this;
	}

	template <
		class G,
		std::enable_if_t<
			std::conjunction_v<
    				std::is_constructible<E, G&&>,
    				std::is_assignable<E&, G&&>
			>,
			bool
		> = false
	>
	constexpr Result& operator=(Error<G>&& e) noexcept(
		std::is_nothrow_assignable_v<E&, G&&>
		&& std::is_nothrow_constructible_v<E, G&&>
	) {
		if(!this->has_value()) {
			this->err() = std::move(e.value());
			return *this;
		}
		new (std::addressof(this->err())) E(std::move(e.value()));
		data_.has_value() = false;
		return *this;
	}

	constexpr void emplace() noexcept {
		if(this->has_value()) {
			return;
		}
		this->destruct_error();
		data_.has_value() = true;
	}

	template <
		class Other = Result,
		std::enable_if_t<
			std::conjunction_v<
				std::is_same<Other, Result>,
				std::is_move_constructible<E>,
				std::is_swappable<E>
			>,
			bool
		> = false
	>
	constexpr void swap(Other& other) noexcept(
		std::conjunction_v<
			std::is_nothrow_move_constructible<E>,
			std::is_nothrow_swappable<E>
		>
	) {
		if(this->has_value()) {
			if(other.has_value()) {
				return;
			} else {
				new (std::addressof(this->error())) E(std::move(other.error()));
				this->data_.has_value() = false;
				other.destruct_error();
				other.data_.has_value() = false;
			}
		} else {
			if(other.has_value()) {
				new (std::addressof(other.error())) E(std::move(this->error()));
				other.data_.has_value() = false;
				this->destruct_error();
				this->data_.has_value() = true;
				
			} else {
				using std::swap;
				swap(this->error(), other.error());
			}
		}
	}

	constexpr bool has_value() const {
		return this->data_.has_value();
	}

	explicit constexpr operator bool() const {
		return this->has_value();
	}

	constexpr void value() const& {
		if(!this->has_value()) {
			this->throw_bad_result_access();
		}
	}

	constexpr void value() && {
		if(!this->has_value()) {
			std::move(*this).throw_bad_result_access();
		}
	}

	constexpr E&& error() && {
		assert_not_has_value();
		return std::move(this->err());
	}

	constexpr const E&& error() const&& {
		assert_not_has_value();
		return std::move(this->err());
	}

	constexpr E& error() & {
		assert_not_has_value();
		return this->err();
	}

	constexpr const E& error() const& {
		assert_not_has_value();
		return this->err();
	}

private:
	constexpr void assert_has_value() const {
#if defined(assert) && !defined(TIM_RESULT_DISABLE_ASSERTIONS)
		assert(this->has_value());
#endif
	}

	constexpr void assert_not_has_value() const {
#if defined(assert) && defined(TIM_RESULT_DISABLE_ASSERTIONS)
		assert(not this->has_value());
#endif
	}

	[[noreturn]]
	void throw_bad_result_access() const& noexcept(false) {
		if constexpr(std::is_move_constructible_v<E>) {
			throw BadResultAccess<E>(this->error());
		} else {
			throw BadResultAccess<E>();
		}
	}
	
	[[noreturn]]
	void throw_bad_result_access() && noexcept(false) {
		if constexpr(std::is_move_constructible_v<E>) {
			throw BadResultAccess<E>(std::move(this->error()));
		} else {
			throw BadResultAccess<E>();
		}
	}
	
	constexpr void destruct_value() noexcept {
		return data_.destruct_value();
	}

	constexpr void destruct_error() noexcept {
		return data_.destruct_error();
	}

	constexpr void destruct() noexcept {
		return data_.destruct();
	}

	constexpr const E& err() const {
		return this->data_.error();
	}

	constexpr E& err() {
		return this->data_.error();
	}

	data_type data_;
};

template <class E>
struct Result<const volatile void, E> {
private:
	template <class U, class G>
	friend struct Result;

	using data_type = detail::result_data_type<const volatile void, E>;
	static constexpr detail::value_tag_t value_tag = detail::value_tag;
	static constexpr detail::error_tag_t error_tag = detail::error_tag;
public:

	using value_type = const volatile void;
	using error_type = E;

	Result() = default;
	Result(const Result&) = default;
	Result(Result&&) = default;

	template <
		class G,
		class U,
		std::enable_if_t<
			!std::is_convertible_v<const G&, E>
			&& std::conjunction_v<
				detail::is_cv_void<U>,
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
	explicit constexpr Result(const Result<U, G>& other) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(value_tag);
			} else {
				return data_type(error_tag, other.err());
			}
		}())
	{
		
	}

	template <
		class U,
		class G,
		std::enable_if_t<
			std::is_convertible_v<const G&, E>
			&& std::conjunction_v<
				detail::is_cv_void<U>,
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
	constexpr Result(const Result<U, G>& other) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(value_tag);
			} else {
				return data_type(error_tag, other.err());
			}
		}())
	{
		
	}

	template <
		class U,
		class G,
		std::enable_if_t<
			!std::is_convertible_v<G&&, E>
			&& std::conjunction_v<
				detail::is_cv_void<U>,
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
	constexpr explicit Result(Result<U, G>&& other) noexcept(std::is_nothrow_constructible_v<E, G&&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(value_tag);
			} else {
				return data_type(error_tag, std::move(other.err()));
			}
		}())
	{
		
	}
	
	template <
		class U,
		class G,
		std::enable_if_t<
			std::is_convertible_v<G&&, E>
			&& std::conjunction_v<
				detail::is_cv_void<U>,
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
	constexpr Result(Result<U, G>&& other) noexcept(std::is_nothrow_constructible_v<E, G&&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(value_tag);
			} else {
				return data_type(error_tag, std::move(other.err()));
			}
		}())
	{
		
	}
	
	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::is_convertible<const G&, E>,
				std::is_constructible<E, const G&>
			>,
			bool
		> = false
	>
	constexpr Result(const Error<G>& v) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_(error_tag, v.value())
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<std::is_convertible<const G&, E>>,
				std::is_constructible<E, const G&>
			>,
			bool
		> = false
	>
	constexpr explicit Result(const Error<G>& v) noexcept(std::is_nothrow_constructible_v<E, const G&>):
		data_(error_tag, v.value())
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::is_convertible<G&&, E>,
				std::is_constructible<E, G&&>
			>,
			bool
		> = false
	>
	constexpr Result(Error<G>&& v) noexcept(std::is_nothrow_constructible_v<E, G&>):
		data_(error_tag, std::move(v.value()))
	{
			
	}

	template <
		class G = E,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<std::is_convertible<G&&, E>>,
				std::is_constructible<E, G&&>
			>,
			bool
		> = false
	>
	constexpr explicit Result(Error<G>&& v) noexcept(std::is_nothrow_constructible_v<E, G&&>):
		data_(error_tag, std::move(v.value()))
	{
			
	}

	constexpr explicit Result(in_place_t) noexcept:
		data_(value_tag)
	{
			
	}

	template <
		class ... Args,
		std::enable_if_t<std::is_constructible_v<E, Args&&...>, bool> = false
	>
	constexpr explicit Result(in_place_error_t, Args&& ... args) noexcept(std::is_nothrow_constructible_v<E, Args&&...>):
		data_(error_tag, std::forward<Args>(args)...)
	{
			
	}

	template <
		class U,
		class ... Args,
		std::enable_if_t<
			std::is_constructible_v<E, std::initializer_list<U>&, Args&&...>,
			bool
		> = false
	>
	constexpr explicit Result(in_place_error_t, std::initializer_list<U> ilist, Args&& ... args) noexcept(
		std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args&&...>
	):
		data_(error_tag, ilist, std::forward<Args>(args)...)
	{
			
	}

	constexpr Result& operator=(const Result&) = default;
	constexpr Result& operator=(Result&&) = default;

	template <
		class G,
		std::enable_if_t<
			std::conjunction_v<
    				std::is_constructible<E, const G&>,
    				std::is_assignable<E&, const G&>
			>,
			bool
		> = false
	>
	constexpr Result& operator=(const Error<G>& e) noexcept(
		std::is_nothrow_assignable_v<E&, const G&>
		&& std::is_nothrow_constructible_v<E, const G&>
	)
	{
		if(!this->has_value()) {
			this->err() = e.value();
			return *this;
		}
		new (std::addressof(this->err())) E(e.value());
		data_.has_value() = false;
		return *this;
	}

	template <
		class G,
		std::enable_if_t<
			std::conjunction_v<
    				std::is_constructible<E, G&&>,
    				std::is_assignable<E&, G&&>
			>,
			bool
		> = false
	>
	constexpr Result& operator=(Error<G>&& e) noexcept(
		std::is_nothrow_assignable_v<E&, G&&>
		&& std::is_nothrow_constructible_v<E, G&&>
	) {
		if(!this->has_value()) {
			this->err() = std::move(e.value());
			return *this;
		}
		new (std::addressof(this->err())) E(std::move(e.value()));
		data_.has_value() = false;
		return *this;
	}

	constexpr void emplace() noexcept {
		if(this->has_value()) {
			return;
		}
		this->destruct_error();
		data_.has_value() = true;
	}

	template <
		class Other = Result,
		std::enable_if_t<
			std::conjunction_v<
				std::is_same<Other, Result>,
				std::is_move_constructible<E>,
				std::is_swappable<E>
			>,
			bool
		> = false
	>
	constexpr void swap(Other& other) noexcept(
		std::conjunction_v<
			std::is_nothrow_move_constructible<E>,
			std::is_nothrow_swappable<E>
		>
	) {
		if(this->has_value()) {
			if(other.has_value()) {
				return;
			} else {
				new (std::addressof(this->error())) E(std::move(other.error()));
				this->data_.has_value() = false;
				other.destruct_error();
				other.data_.has_value() = false;
			}
		} else {
			if(other.has_value()) {
				new (std::addressof(other.error())) E(std::move(this->error()));
				other.data_.has_value() = false;
				this->destruct_error();
				this->data_.has_value() = true;
				
			} else {
				using std::swap;
				swap(this->error(), other.error());
			}
		}
	}

	constexpr bool has_value() const {
		return this->data_.has_value();
	}

	explicit constexpr operator bool() const {
		return this->has_value();
	}

	constexpr void value() const& {
		if(!this->has_value()) {
			this->throw_bad_result_access();
		}
	}

	constexpr void value() && {
		if(!this->has_value()) {
			std::move(*this).throw_bad_result_access();
		}
	}

	constexpr E&& error() && {
		assert_not_has_value();
		return std::move(this->err());
	}

	constexpr const E&& error() const&& {
		assert_not_has_value();
		return std::move(this->err());
	}

	constexpr E& error() & {
		assert_not_has_value();
		return this->err();
	}

	constexpr const E& error() const& {
		assert_not_has_value();
		return this->err();
	}

private:
	constexpr void assert_has_value() const {
#if defined(assert) && !defined(TIM_RESULT_DISABLE_ASSERTIONS)
		assert(this->has_value());
#endif
	}

	constexpr void assert_not_has_value() const {
#if defined(assert) && defined(TIM_RESULT_DISABLE_ASSERTIONS)
		assert(not this->has_value());
#endif
	}

	[[noreturn]]
	void throw_bad_result_access() const& noexcept(false) {
		if constexpr(std::is_move_constructible_v<E>) {
			throw BadResultAccess<E>(this->error());
		} else {
			throw BadResultAccess<E>();
		}
	}
	
	[[noreturn]]
	void throw_bad_result_access() && noexcept(false) {
		if constexpr(std::is_move_constructible_v<E>) {
			throw BadResultAccess<E>(std::move(this->error()));
		} else {
			throw BadResultAccess<E>();
		}
	}
	
	constexpr void destruct_value() noexcept {
		return data_.destruct_value();
	}

	constexpr void destruct_error() noexcept {
		return data_.destruct_error();
	}

	constexpr void destruct() noexcept {
		return data_.destruct();
	}

	constexpr const E& err() const {
		return this->data_.error();
	}

	constexpr E& err() {
		return this->data_.error();
	}

	data_type data_;
};

namespace traits::detail {

template <class R1, class R2>
struct results_are_equality_comparable{};

template <class T1, class E1, class T2, class E2>
struct results_are_equality_comparable<Result<T1, E1>, Result<T2, E2>> {
	template <class T>
	struct Tag {};

	template <class T, class U, class R1 = decltype(std::declval<const T&>() == std::declval<const U&>())>
	static constexpr auto is_eq_comp(Tag<T>, Tag<U>, int)
		-> std::conditional_t<
			is_contextually_convertible_to_bool_v<R1>,
			std::true_type,
			std::false_type
		>
	{
		return std::conditional_t<
			is_contextually_convertible_to_bool_v<R1>,
			std::true_type,
			std::false_type
		>{};
	}
	
	template <class T, class U>
	static constexpr std::false_type is_eq_comp(Tag<T>, Tag<U>, ...) { return std::false_type{}; }
	
	static constexpr bool value = std::conjunction_v<
		std::disjunction<
			std::conjunction<
				::tim::result::detail::is_cv_void<T1>,
				::tim::result::detail::is_cv_void<T2>
			>,
			decltype(is_eq_comp(Tag<T1>{}, Tag<T2>{}, 0))
		>,
		decltype(is_eq_comp(Tag<E1>{}, Tag<E2>{}, 0))
	>;
};

template <class R1, class R2>
inline constexpr bool results_are_equality_comparable_v
	= results_are_equality_comparable<R1, R2>::value;

template <class R1, class R2>
struct results_are_inequality_comparable{};
template <class T1, class E1, class T2, class E2>
struct results_are_inequality_comparable<Result<T1, E1>, Result<T2, E2>> {
	template <class T>
	struct Tag {};

	template <class T, class U, class R1 = decltype(std::declval<const T&>() != std::declval<const U&>())>
	static constexpr auto is_eq_comp(Tag<T>, Tag<U>, int)
		-> std::conditional_t<
			is_contextually_convertible_to_bool_v<R1>,
			std::true_type,
			std::false_type
		>
	{
		return std::conditional_t<
			is_contextually_convertible_to_bool_v<R1>,
			std::true_type,
			std::false_type
		>{};
	}
	
	template <class T, class U>
	static constexpr std::false_type is_eq_comp(Tag<T>, Tag<U>, ...) { return std::false_type{}; }
	
	static constexpr bool value = std::conjunction_v<
		std::disjunction<
			std::conjunction<
				::tim::result::detail::is_cv_void<T1>,
				::tim::result::detail::is_cv_void<T2>
			>,
			decltype(is_eq_comp(Tag<T1>{}, Tag<T2>{}, 0))
		>,
		decltype(is_eq_comp(Tag<E1>{}, Tag<E2>{}, 0))
	>;
};

template <class R1, class R2>
inline constexpr bool results_are_inequality_comparable_v
	= results_are_inequality_comparable<R1, R2>::value;


} /* namespace traits::detail */

/* --- Equality Operators --- */
template <
	class T1, class E1,
	class T2, class E2,
	std::enable_if_t<
		traits::detail::results_are_equality_comparable_v<Result<T1, E1>, Result<T2, E2>>,
		bool
	> = false
>
constexpr bool operator==(const Result<T1, E1>& lhs, const Result<T2, E2>& rhs) {
	if(lhs.has_value()) {
		if(rhs.has_value()) {
			if constexpr(!detail::is_cv_void_v<T1>) {
				return *lhs == *rhs;
			} else {
				return true;
			}
		}
		return false;
	} else {
		if(!rhs.has_value()) {
			return lhs.error() == rhs.error();
		}
		return false;
	}
}

template <
	class T1,
	class E1,
	class T2,
	std::enable_if_t<
		!detail::is_cv_void_v<T1>
		&& traits::detail::is_contextually_convertible_to_bool_v<decltype(std::declval<const T1&>() == std::declval<T2&&>())>
		&& !traits::is_result_v<std::decay_t<T2>>
		&& !traits::is_error_v<std::decay_t<T2>>,
		bool
	> = false
>
constexpr bool operator==(const Result<T1, E1>& lhs, T2&& rhs) {
	if(lhs.has_value()) {
		return *lhs == std::forward<T2>(rhs);
	} else {
		return false;
	}
}

template <
	class T1,
	class T2,
	class E2,
	std::enable_if_t<
		!detail::is_cv_void_v<T2>
		&& traits::detail::is_contextually_convertible_to_bool_v<decltype(std::declval<T1&&>() == std::declval<const T2&>())>
		&& !traits::is_result_v<T1>
		&& !traits::is_error_v<T1>,
		bool
	> = false
>
constexpr bool operator==(T1&& lhs, const Result<T2, E2>& rhs) {
	if(rhs.has_value()) {
		return std::forward<T1>(lhs) == *rhs;
	} else {
		return false;
	}
}

template <
	class T1,
	class E1,
	class E2,
	std::enable_if_t<
		traits::detail::is_contextually_convertible_to_bool_v<decltype(std::declval<const E1&>() == std::declval<const E2&>())>,
		bool
	> = false
>
constexpr bool operator==(const Result<T1, E1>& lhs, const Error<E2>& rhs) {
	if(lhs.has_value()) {
		return false;
	}
	return lhs.error() == rhs.value();
}

template <
	class E1,
	class T2,
	class E2,
	std::enable_if_t<
		traits::detail::is_contextually_convertible_to_bool_v<decltype(std::declval<const E1&>() == std::declval<const E2&>())>,
		bool
	> = false
>
constexpr bool operator==(const Error<E1>& lhs, const Result<T2, E2>& rhs) {
	if(rhs.has_value()) {
		return false;
	}
	return lhs.value() == rhs.error();
}

/* --- Inequality Operators --- */
template <
	class T1, class E1,
	class T2, class E2,
	std::enable_if_t<
		traits::detail::results_are_inequality_comparable_v<Result<T1, E1>, Result<T2, E2>>,
		bool
	> = false
>
constexpr bool operator!=(const Result<T1, E1>& lhs, const Result<T2, E2>& rhs) {
	static_assert(detail::is_cv_void_v<T1> == detail::is_cv_void_v<T2>);
	if(lhs.has_value()) {
		if(rhs.has_value()) {
			if constexpr(!detail::is_cv_void_v<T1>) {
				return *lhs != *rhs;
			} else {
				return false;
			}
		}
		return true;
	} else {
		if(!rhs.has_value()) {
			return lhs.error() != rhs.error();
		}
		return true;
	}
}

template <
	class T1,
	class E1,
	class T2,
	std::enable_if_t<
		!detail::is_cv_void_v<T1>
		&& traits::detail::is_contextually_convertible_to_bool_v<decltype(std::declval<const T1&>() != std::declval<T2&&>())>
		&& !traits::is_result_v<std::decay_t<T2>>
		&& !traits::is_error_v<std::decay_t<T2>>,
		bool
	> = false
>
constexpr bool operator!=(const Result<T1, E1>& lhs, T2&& rhs) {
	if(lhs.has_value()) {
		return *lhs != std::forward<T2>(rhs);
	} else {
		return true;
	}
}

template <
	class T1,
	class T2,
	class E2,
	std::enable_if_t<
		!detail::is_cv_void_v<T2>
		&& traits::detail::is_contextually_convertible_to_bool_v<decltype(std::declval<T1&&>() != std::declval<const T2&>())>
		&& !traits::is_result_v<T1>
		&& !traits::is_error_v<T1>,
		bool
	> = false
>
constexpr bool operator!=(T1&& lhs, const Result<T2, E2>& rhs) {
	if(rhs.has_value()) {
		return std::forward<T1>(lhs) != *rhs;
	} else {
		return true;
	}
}

template <
	class T1,
	class E1,
	class E2,
	std::enable_if_t<
		traits::detail::is_contextually_convertible_to_bool_v<decltype(std::declval<const E1&>() != std::declval<const E2&>())>,
		bool
	> = false
>
constexpr bool operator!=(const Result<T1, E1>& lhs, const Error<E2>& rhs) {
	if(lhs.has_value()) {
		return true;
	}
	return lhs.error() != rhs.value();
}

template <
	class E1,
	class T2,
	class E2,
	std::enable_if_t<
		traits::detail::is_contextually_convertible_to_bool_v<decltype(std::declval<const E1&>() != std::declval<const E2&>())>,
		bool
	> = false
>
constexpr bool operator!=(const Error<E1>& lhs, const Result<T2, E2>& rhs) {
	if(rhs.has_value()) {
		return true;
	}
	return lhs.value() != rhs.error();
}

template <
	class T,
	class E,
	std::enable_if_t<
		std::conjunction_v<
			std::disjunction<
				detail::is_cv_void<T>,
				std::is_move_constructible<T>
			>,
			std::is_swappable<T>,
			std::is_move_constructible<E>,
			std::is_swappable<E>
		>,
		bool
	> = false
>
constexpr void swap(Result<T, E>& lhs, Result<T, E>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
	return lhs.swap(rhs);
}

} /* inline namespace result */

} /* namespace tim */

#endif /* TIM_RESULT_HPP */

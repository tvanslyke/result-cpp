#include "catch.hpp"
#include "tim/result/Result.hpp"

struct NoMove {
	NoMove() = default;
	NoMove(const NoMove&) = default;
	NoMove& operator=(const NoMove&) = default;
	NoMove(NoMove&&) = delete;
	NoMove& operator=(NoMove&&) = delete;
};

struct NoCopy {
	NoCopy() = default;
	NoCopy(const NoCopy&) = delete;
	NoCopy& operator=(const NoCopy&) = delete;
	NoCopy(NoCopy&&) = default;
	NoCopy& operator=(NoCopy&&) = default;
};

struct NoMoveOrCopy {
	NoMoveOrCopy() = default;
	NoMoveOrCopy(const NoMoveOrCopy&) = delete;
	NoMoveOrCopy& operator=(const NoMoveOrCopy&) = delete;
	NoMoveOrCopy(NoMoveOrCopy&&) = delete;
	NoMoveOrCopy& operator=(NoMoveOrCopy&&) = delete;
};

struct Dummy{};

TEST_CASE("Bad Result Access", "[BadResultAccess]") {
	static_assert(std::is_base_of<std::exception, tim::BadResultAccess<>>::value,
	              "");
	static_assert(std::is_base_of<tim::BadResultAccess<>, tim::BadResultAccess<int, true>>::value,
	              "");
	static_assert(std::is_base_of<tim::BadResultAccess<int, false>, tim::BadResultAccess<int, true>>::value,
	              "");
	static_assert(noexcept(tim::BadResultAccess<>{}), "must be noexcept");
	static_assert(noexcept(tim::BadResultAccess<int, false>{}.what()), "must be noexcept");
	static_assert(noexcept(tim::BadResultAccess<int, true>{}.what()), "must be noexcept");

	{
		tim::BadResultAccess<> ex;
		REQUIRE(ex.what());
	}

	{
		try {
			tim::Result<int, int> res(tim::Error(42));
			res.value();
			REQUIRE(false);
		} catch(const tim::BadResultAccess<int, true>& e) {
			REQUIRE(e.what());
			REQUIRE(e.error());
			REQUIRE(e.error() == 42);
			REQUIRE(dynamic_cast<const tim::BadResultAccess<int>*>(&e));
		}
	}
	{
		try {
			tim::Result<void, int> res(tim::Error(42));
			res.value();
			REQUIRE(false);
		} catch(const tim::BadResultAccess<int, true>& e) {
			REQUIRE(e.what());
			REQUIRE(e.error());
			REQUIRE(e.error() == 42);
			REQUIRE(dynamic_cast<const tim::BadResultAccess<int>*>(&e));
		}
	}
	{
		try {
			tim::Result<int, Dummy> res(tim::in_place_error);
			res.value();
			REQUIRE(false);
		} catch(const tim::BadResultAccess<Dummy, true>& e) {
			REQUIRE(e.what());
			e.error();
		}
	}
	{
		try {
			tim::Result<void, Dummy> res(tim::in_place_error);
			res.value();
			REQUIRE(false);
		} catch(const tim::BadResultAccess<Dummy, true>& e) {
			REQUIRE(e.what());
			e.error();
		}
	}

}

TEST_CASE("Bad Result Access Noncopyable Error", "[BadResultAccess]") {
	{
		try {
			tim::Result<int, NoCopy> res(tim::in_place_error);
			res.value();
			REQUIRE(false);
		} catch(const tim::BadResultAccess<NoCopy, true>& e) {
			REQUIRE(false);
		} catch(const tim::BadResultAccess<NoCopy, false>& e) {
			REQUIRE(e.what());
		}
	}
	{
		try {
			tim::Result<void, NoCopy> res(tim::in_place_error);
			std::move(res).value();
			REQUIRE(false);
		} catch(const tim::BadResultAccess<NoCopy, true>& e) {
			REQUIRE(e.what());
			e.error();
		}
	}
}

TEST_CASE("Bad Result Access Nonmovable Error", "[BadResultAccess]") {
	{
		try {
			tim::Result<int, NoMove> res(tim::in_place_error);
			res.value();
			REQUIRE(false);
		} catch(const tim::BadResultAccess<NoMove, true>& e) {
			REQUIRE(e.what());
			e.error();
		}
	}
	{
		try {
			tim::Result<void, NoMove> res(tim::in_place_error);
			std::move(res).value();
			REQUIRE(false);
		} catch(const tim::BadResultAccess<NoMove, true>& e) {
			REQUIRE(e.what());
			// Still works because overload resolution selects the copy constructor.
			e.error();
		}
	}
}

TEST_CASE("Bad Result Access Noncopyable and Nonmovable Error", "[BadResultAccess]") {
	{
		try {
			tim::Result<int, NoMoveOrCopy> res(tim::in_place_error);
			res.value();
			REQUIRE(false);
		} catch(const tim::BadResultAccess<NoMoveOrCopy, true>& e) {
			REQUIRE(false);
		} catch(const tim::BadResultAccess<NoMoveOrCopy>& e) {
			REQUIRE(e.what());
		}
	}
	{
		try {
			tim::Result<void, NoMoveOrCopy> res(tim::in_place_error);
			std::move(res).value();
			REQUIRE(false);
		} catch(const tim::BadResultAccess<NoMoveOrCopy, true>& e) {
			REQUIRE(false);
		} catch(const tim::BadResultAccess<NoMoveOrCopy>& e) {
			REQUIRE(e.what());
		}
	}
}


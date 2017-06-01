/**
 * Copyright Bogumił Chojnowski 2017
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt
 */

#include "hsm/hsm.hpp"

#include <gmock/gmock.h>

namespace hsm
{
using namespace testing;

namespace
{
struct CallMock
{
    MOCK_METHOD0(call, bool());
};
std::unique_ptr<CallMock> call_mock = nullptr;
}  // namespace

struct some_guard
{
    template <class sm>
    bool operator()(sm const&, int const&, char const&, int const&)
    {
        return call_mock->call();
    }
};

struct StateMachineGuardTest : Test
{
    void SetUp() override { call_mock = std::make_unique<CallMock>(); }
    void TearDown() override
    {
        EXPECT_TRUE(Mock::VerifyAndClearExpectations(call_mock.get()));
        call_mock.reset();
    }

    using sm = hsm::state_machine<
        char,
        hsm::rows_t<hsm::row_t<char, int, int, empty_action, some_guard>>>;

    sm sut;
};

TEST_F(StateMachineGuardTest, test_guard_passing)
{
    EXPECT_CALL(*call_mock, call()).WillOnce(Return(true));

    ASSERT_NO_THROW(sut.process_event(int{}));
}

TEST_F(StateMachineGuardTest, test_guard_blocking)
{
    EXPECT_CALL(*call_mock, call()).WillOnce(Return(false));

    ASSERT_THROW(sut.process_event(int{}), guard_reject);
}
}  // namespace hsm

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
struct
{
    MOCK_METHOD0(call, bool());
} call_mock;
}  // namespace

struct some_guard
{
    template <class sm>
    bool operator()(sm const&, int const&, char const&, int const&)
    {
        return call_mock.call();
    }
};

struct StateMachineGuardTest : Test
{
    void TearDown() override { Mock::VerifyAndClearExpectations(&call_mock); }

    using sm = hsm::state_machine<
        char,
        hsm::rows_t<hsm::row_t<char, int, int, empty_action, some_guard>>>;

    sm sut;
};

TEST_F(StateMachineGuardTest, test_guard_passing)
{
    EXPECT_CALL(call_mock, call()).WillOnce(Return(true));

    ASSERT_NO_THROW(sut.process_event(int{}));
}

TEST_F(StateMachineGuardTest, test_guard_blocking)
{
    EXPECT_CALL(call_mock, call()).WillOnce(Return(false));

    ASSERT_THROW(sut.process_event(int{}), guard_reject);
}
}  // namespace hsm

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
    MOCK_METHOD0(call, void());
};
std::unique_ptr<CallMock> call_mock = nullptr;
}  // namespace

struct some_guard
{
    template <class sm>
    void operator()(sm const&, int const&, char const&, int const&)
    {
        call_mock->call();
    }
};

struct StateMachineActionTest : Test
{
    void SetUp() override
    {
        call_mock = std::make_unique<CallMock>();
        EXPECT_CALL(*call_mock, call()).Times(1);
    }
    void TearDown() override
    {
        EXPECT_TRUE(Mock::VerifyAndClearExpectations(call_mock.get()));
        call_mock.reset();
    }

    using sm =
        hsm::state_machine<char,
                           hsm::rows_t<hsm::row_t<char, int, int, some_guard>>>;

    sm sut;
};

TEST_F(StateMachineActionTest, test_action)
{
    ASSERT_NO_THROW(sut.process_event(int{}));
}
}  // namespace hsm

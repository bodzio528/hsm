/**
 * Copyright Bogumił Chojnowski 2017
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt
 */

#include "hsm/hsm.hpp"

#include <gtest/gtest.h>

namespace hsm
{
using namespace testing;

TEST(StateMachineTest, primitive_events_can_be_used_as_transition_triggers)
{
    using sm = hsm::state_machine<int,
                                  hsm::rows_t<hsm::row_t<int, char, int>,
                                              hsm::row_t<int, int, int>,
                                              hsm::row_t<int, float, int>>>;
    sm sut;

    sut.process_event(3);
    sut.process_event('c');
    // 5.0 is a double, not float:
    ASSERT_THROW(sut.process_event(5.0), event_not_supported_exception);
    ASSERT_NO_THROW(sut.process_event(3.0f));
}

TEST(StateMachineTest, userdefined_initial_state_is_other_than_zeroth)
{
    using sm = hsm::state_machine<float,
                                  hsm::rows_t<hsm::row_t<char, int, int>,
                                              hsm::row_t<int, int, float>,
                                              hsm::row_t<float, int, double>>>;
    sm sut;
    sut.process_event(3);
}

TEST(StateMachineTest, DISABLED_incoming_unexpected_event_throws_exception)
{
    using sm = hsm::state_machine<int, hsm::rows_t<hsm::row_t<char, int, int>>>;
    sm sut;
    ASSERT_THROW(sut.process_event(3.0), std::exception);
}
}  // namespace hsm

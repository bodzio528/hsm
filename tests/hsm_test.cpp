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

    ASSERT_NO_THROW(sut.process_event(int{}));
    ASSERT_NO_THROW(sut.process_event(char{}));
    ASSERT_NO_THROW(sut.process_event(float{}));
}

TEST(StateMachineTest, initial_state_can_be_defined_by_user)
{
    using sm = hsm::state_machine<float,
                                  hsm::rows_t<hsm::row_t<char, int, int>,
                                              hsm::row_t<int, int, float>,
                                              hsm::row_t<float, int, double>>>;
    sm sut;
    ASSERT_NO_THROW(sut.process_event(int{}));
}

TEST(StateMachineTest, when_incomming_unsupported_event__then_throw_exception)
{
    using sm = hsm::state_machine<int, hsm::rows_t<hsm::row_t<int, char, int>>>;
    sm sut;

    ASSERT_THROW(sut.process_event(double{}), unsupported_event);
}

TEST(StateMachineTest, when_incoming_known_unexpected_event__then_throw_exception)
{
    using sm = hsm::state_machine<
        int,
        hsm::rows_t<hsm::row_t<char, int, int>, hsm::row_t<int, float, int>>>;
    sm sut;
    ASSERT_THROW(sut.process_event(int{}), unexpected_event);
}

TEST(StateMachineTest,
     when_transitions_conflict__then_first_in_apperance_is_taken)
{
    using sm = hsm::state_machine<int,
                                  hsm::rows_t<hsm::row_t<char, int, int>,
                                              hsm::row_t<int, int, float>,
                                              hsm::row_t<int, int, double>>>;
    sm sut;
    ASSERT_NO_THROW(sut.process_event(int{}));
}

}  // namespace hsm

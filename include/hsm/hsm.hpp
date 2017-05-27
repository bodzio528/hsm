/**
 * Copyright Bogumił Chojnowski 2017
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <iostream>

#include <boost/hana.hpp>
#include <boost/hana/experimental/printable.hpp>
#include <boost/hana/fwd/tuple.hpp>

#include "hsm_exceptions.hpp"

namespace hsm
{
namespace hana = boost::hana;

using namespace hana::literals;

template <class state, class event, class next>
struct row_t
{
    using state_t = state;
    using next_t = next;
    using event_t = event;
};

template <class... r>
using rows_t = decltype(hana::tuple_t<r...>);

template <class initial_state, class transition_table>
class state_machine
{
    using stt = transition_table;

    template <class row>
    struct extract_event
    {
        using type = typename row::event_t;
    };

    using events_t = decltype(hana::unique(
        hana::transform(stt{}, hana::metafunction<extract_event>)));

    template <class row>
    struct extract_origin_state
    {
        using type = typename row::state_t;
    };

    template <class row>
    struct extract_next_state
    {
        using type = typename row::next_t;
    };

    using aux_unique_states_t = decltype(hana::to_tuple(hana::union_(
        hana::to_set(
            hana::transform(stt{}, hana::metafunction<extract_origin_state>)),
        hana::to_set(
            hana::transform(stt{}, hana::metafunction<extract_next_state>)))));

    //! std::tuple<state0, state1, ..., stateN> : stateI != stateJ for I != J
    using states_t = typename decltype(
        hana::unpack(aux_unique_states_t{}, hana::template_<std::tuple>))::type;

    states_t m_states;
    int m_current_state;

    template <class state>
    static constexpr int get_state_index() noexcept
    {
        // (state_type x index)
        auto states_indices_map = hana::to_map(hana::transform(
            hana::zip(aux_unique_states_t{},
                      hana::to_tuple(hana::make_range(
                          0_c, hana::length(aux_unique_states_t{})))),
            [](auto&& tup) {
                return hana::make_pair(hana::at_c<0>(tup), hana::at_c<1>(tup));
            }));

        std::cout << hana::experimental::print(states_indices_map) << std::endl;
        return *hana::find(states_indices_map, hana::type_c<state>);
    }

public:
    state_machine() : m_current_state(get_state_index<initial_state>()) {}

    template <class event>
    void process_event(event&& e)
    {
        std::cout << "CURRENT_STATE INDEX = " << m_current_state << std::endl;
        if (not hana::contains(events_t{}, hana::type_c<event>)) {
            // this event is not supported within this state machine.
            // todo: check it at compile time
            throw event_not_supported_exception();
        } else {
            // search for transitions including event
            // when found, check at runtime if current state index equals origin
            // state of found transitions  if yes, fire it
            // return invalid state otherwise
        }
    }
};
}  // namespace hsm

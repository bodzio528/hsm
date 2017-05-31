/**
 * Copyright Bogumił Chojnowski 2017
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <experimental/optional>

#include <boost/hana.hpp>

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
        //! (state_type => index)
        auto states_indices_map = hana::to_map(hana::transform(
            hana::zip(aux_unique_states_t{},
                      hana::to_tuple(hana::make_range(
                          0_c, hana::length(aux_unique_states_t{})))),
            [](auto&& tup) {
                return hana::make_pair(hana::at_c<0>(tup), hana::at_c<1>(tup));
            }));
        return *hana::find(states_indices_map, hana::type_c<state>);
    }

public:
    state_machine() : m_current_state(get_state_index<initial_state>()) {}

    template <class event>
    void process_event(event&&)
    {
        auto event_transitions = hana::filter(stt{}, [](auto&& tt) {
            return hana::metafunction<extract_event>(tt) == hana::type_c<event>;
        });

        if (hana::is_empty(event_transitions)) {
            throw unsupported_event();
        }

        bool transition_found = false;
        hana::for_each(event_transitions, [this, &transition_found](auto&& tt) {
            using origin_state = typename decltype(
                hana::metafunction<extract_origin_state>(tt))::type;
            using next_state = typename decltype(
                hana::metafunction<extract_next_state>(tt))::type;

            if (not transition_found and
                m_current_state == state_machine::get_state_index<origin_state>()) {
                if (true /*TODO check guard G: fsm event origin next -> bool */) {
                    transition_found = true;
                    /* TODO call action A: fsm event origin next -> void */
                    m_current_state = state_machine::get_state_index<next_state>();
                }
            }
        });

        if (not transition_found) {
            throw unexpected_event();
        }
    }
};
}  // namespace hsm

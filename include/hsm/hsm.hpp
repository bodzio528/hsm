/**
 * Copyright Bogumił Chojnowski 2017
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <experimental/optional>

#include <boost/hana.hpp>
//#include <boost/hana/experimental/printable.hpp>
#include <boost/hana/ext/std/tuple.hpp>

#include "hsm_exceptions.hpp"

namespace hsm
{
namespace hana = boost::hana;

using namespace hana::literals;

struct empty_guard
{
    template <class state_machine, class event, class state, class next>
    bool operator()(state_machine const&,
                    event const&,
                    state const&,
                    next const&) const
    { /*
         std::cout << "CALLED EMPTY_GUARD: "
                   << " event = "
                   << hana::experimental::print(hana::type_c<event>)
                   << " state = "
                   << hana::experimental::print(hana::type_c<state>)
                   << " next  = "
                   << hana::experimental::print(hana::type_c<next>) <<
         std::endl;*/
        return true;
    }
};

struct empty_action
{
    template <class state_machine, class event, class state, class next>
    void operator()(state_machine const&,
                    event const&,
                    state const&,
                    next const&)
    { /*
         std::cout << "CALLED EMPTY_ACTION:"
                   << " event = "
                   << hana::experimental::print(hana::type_c<event>)
                   << " state = "
                   << hana::experimental::print(hana::type_c<state>)
                   << " next  = "
                   << hana::experimental::print(hana::type_c<next>) <<
         std::endl;*/
    }
};

template <class state,
          class event,
          class next,
          class action = empty_action,
          class guard = empty_guard>
struct row_t
{
    using state_t = state;
    using next_t = next;
    using event_t = event;
    using action_t = action;
    using guard_t = guard;
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

    template <class row>
    struct extract_action
    {
        using type = typename row::action_t;
    };

    template <class row>
    struct extract_guard
    {
        using type = typename row::guard_t;
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
    void process_event(event&& evt)
    {
        auto event_transitions = hana::filter(stt{}, [](auto&& tt) {
            return hana::metafunction<extract_event>(tt) == hana::type_c<event>;
        });

        if (hana::is_empty(event_transitions)) {
            throw unsupported_event();
        }

        bool transition_found = false;
        hana::for_each(
            event_transitions, [this, &transition_found, &evt](auto&& tt) {
                using origin_state = typename decltype(
                    hana::metafunction<extract_origin_state>(tt))::type;

                using next_state = typename decltype(
                    hana::metafunction<extract_next_state>(tt))::type;

                using action = typename decltype(
                    hana::metafunction<extract_action>(tt))::type;

                using guard = typename decltype(
                    hana::metafunction<extract_guard>(tt))::type;

                if (not transition_found and
                    m_current_state ==
                        state_machine::get_state_index<origin_state>()) {
                    if (guard()(*this,
                                evt,
                                std::get<origin_state>(m_states),
                                std::get<next_state>(m_states))) {
                        transition_found = true;
                        action()(*this,
                                 evt,
                                 std::get<origin_state>(m_states),
                                 std::get<next_state>(m_states));
                        m_current_state =
                            state_machine::get_state_index<next_state>();
                    } else {
                        throw guard_reject();
                    }
                }
            });

        if (not transition_found) {
            throw unexpected_event();
        }
    }
};
}  // namespace hsm

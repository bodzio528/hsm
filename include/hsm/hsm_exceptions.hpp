/**
 * Copyright Bogumił Chojnowski 2017
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <exception>

namespace hsm
{
struct event_not_supported_exception : std::exception
{
    virtual const char* what() const noexcept
    {
        return "event not supported (rt)";
    }
};

}  // namespace hsm

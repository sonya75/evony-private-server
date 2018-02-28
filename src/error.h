/* Copyright (C) Daisy - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Daisy <daisy@spitfire.pw>, February 2018
*/

#pragma once

#include <exception>
#include <string>
#include <utility>
#include <system_error>


using err_str_pair = std::pair<std::error_code, std::string>;

using error_code = std::error_code;

enum error_value
{
    /// Catch-all library error
    general = 1,

    /// Does not have permission to perform this action
    no_permission,

    /// Feature not yet implemented
    not_implemented,

    max_errors

};

class category : public std::error_category
{
public:
    category() = default;

    char const * name() const noexcept override
    {
        return "spitfire";
    }

    std::string message(int value) const override
    {
        switch (value)
        {
            case error_value::general:
                return "Generic error";
            case error_value::no_permission:
                return "No permission for this action";
            case error_value::not_implemented:
                return "Feature not yet implemented";
            default:
                return "Unknown";
        }
    }
};

inline const std::error_category & get_category()
{
    static category instance;
    return instance;
}

inline std::error_code make_error_code(error_value e)
{
    return std::error_code(static_cast<int>(e), get_category());
}


class exception : public std::exception
{
public:
    exception(std::string const & msg, std::error_code ec = make_error_code(error_value::general))
        : _msg(msg.empty() ? ec.message() : msg)
        , _code(ec)
    {
    }

    explicit exception(std::error_code ec)
        : _msg(ec.message())
        , _code(ec)
    {
    }

    ~exception() = default;

    virtual char const * what() const noexcept override
    {
        return _msg.c_str();
    }

    std::error_code code() const noexcept
    {
        return _code;
    }

    const std::string _msg;
    std::error_code _code;
};

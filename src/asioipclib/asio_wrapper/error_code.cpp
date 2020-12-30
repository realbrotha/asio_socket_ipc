//
// Created by realbro on 19. 6. 18.
//

#include "error_code.h"

#include <boost/lexical_cast.hpp>

namespace network {


const char *error_category::name() const {
    return "network";
}

std::string error_category::message(int ev) const {
#define error_code_string(code) \
    case error_code::code: return #code

    switch(static_cast<error_code::enum_type>(ev))
    {
        error_code_string(success);
        error_code_string(callback_is_null);
    }
#undef error_code_string

    std::string unknown_message = "unknown_error(";
    unknown_message += boost::lexical_cast<std::string>(ev);
    unknown_message += ")";
    return unknown_message;
}

error_category& error_category::get_category() {
    static error_category cat;
    return cat;
}

boost::system::error_code set_error_code(error_code val)
{
    return boost::system::error_code(val.get_native_value_(), error_category::get_category());
}

boost::system::error_code set_error_code(boost::system::error_code& ec, error_code val)
{
    ec.assign(val.get_native_value_(), error_category::get_category());
    return ec;
}

} // namespace network

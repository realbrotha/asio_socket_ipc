//
// Created by realbro on 19. 6. 18.
//

#ifndef UTIL_ERROR_CODE_H
#define UTIL_ERROR_CODE_H

#include "boost/system/error_code.hpp"
#include "boost/core/scoped_enum.hpp"

namespace network {


BOOST_SCOPED_ENUM_DECLARE_BEGIN(error_code)
{
    success = 0,
    callback_is_null,
    session_creator_is_null,
    session_is_null,
}
BOOST_SCOPED_ENUM_DECLARE_END(error_code)



boost::system::error_code set_error_code(error_code val);
boost::system::error_code set_error_code(boost::system::error_code& ec, error_code val);

class error_category : public boost::system::error_category
{
public:
    virtual const char *name() const;
    virtual std::string message(int ev) const;
    static error_category& get_category();
};


} // namespace network
#endif //UTIL_ERROR_CODE_H

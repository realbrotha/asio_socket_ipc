//
// Created by realbro on 19. 6. 20.
//

#ifndef UTIL_NETWORK_SSL_INFO_H
#define UTIL_NETWORK_SSL_INFO_H

#include "typedef.h"

namespace network {


class ssl_info
{
public:
    ssl_info();
    ssl_info(const ssl_method m);
    ssl_info(ssl_context& context);
    ~ssl_info();

    boost::system::error_code get_last_error_code() const;

    boost::system::error_code set_info(
        const std::string& cert_path,
        const std::string& key_path,
        const std::string& password);

    boost::system::error_code set_default_option();
    boost::system::error_code set_option(ssl_context::options options);


    ssl_context& get_context();


private:
    std::string password_callback() const;

private:
    boost::system::error_code ec_;

    ssl_context* __context;
    ssl_context& context_;
    std::string password_;
};

} // namespace network
#endif //UTIL_SSL_INFO_H

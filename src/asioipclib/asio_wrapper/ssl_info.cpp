//
// Created by realbro on 19. 6. 20.
//

#include "ssl_info.h"

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

namespace network {


ssl_info::ssl_info()
: __context(new ssl_context(ssl_context::sslv23))
, context_(*__context)
{}

ssl_info::ssl_info(const ssl_method m)
: __context(new ssl_context(m))
, context_(*__context)
{}

ssl_info::ssl_info(ssl_context& context)
: __context(NULL)
, context_(context)
{}

ssl_info::~ssl_info()
{
    if (__context)
    {
        delete __context;
    }
}

boost::system::error_code ssl_info::get_last_error_code() const
{
    return ec_;
}

boost::system::error_code ssl_info::set_info(
    const std::string& cert_path,
    const std::string& key_path,
    const std::string& password)
{
    password_ = password;
    try
    {
        // 순서 바뀌면 chain-file 패스워드 물어봄.
        context_.set_password_callback(boost::bind(&ssl_info::password_callback, this));
        context_.use_certificate_chain_file(cert_path);
        context_.use_private_key_file(key_path, network::ssl_context::pem);
    } catch (const boost::system::system_error& ex)
    {
        ec_ = ex.code();
        return ec_;
    }
}

boost::system::error_code ssl_info::set_default_option()
{
    return set_option(boost::asio::ssl::context::default_workarounds |
        boost::asio::ssl::context::no_sslv2 |
        boost::asio::ssl::context::single_dh_use);
}

boost::system::error_code ssl_info::set_option(ssl_context::options options)
{
    ec_.clear();
    context_.set_options(options, ec_);
    return ec_;
}

ssl_context& ssl_info::get_context()
{
    return context_;
}

std::string ssl_info::password_callback() const
{
    return password_;
}


} // namespace network

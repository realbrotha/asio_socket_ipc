//
// Created by realbro on 19. 6. 24.
//

#include "tcp_pack.h"

#include <iostream>
namespace network {


tcp_pack::tcp_pack()
: io__(new io_context)
, io_(*io__)
{
    init();
}

tcp_pack::tcp_pack(io_context& io)
: io__(NULL)
, io_(io)
{
    init();
}

tcp_pack::tcp_pack(const ssl_method m)
: io__(new io_context)
, io_(*io__)
, ssl_info_(new ssl_info(m))
{
    init(true);
}

tcp_pack::tcp_pack(io_context& io, const ssl_method m)
: io__(NULL)
, io_(io)
, ssl_info_(new ssl_info(m))
{
    init(true);
}

tcp_pack::tcp_pack(io_context& io, ssl_context& context)
: io__(NULL)
, io_(io)
, ssl_info_(new ssl_info(context))
{
    init(true);
}

tcp_pack::~tcp_pack()
{
    ssl_socket_.reset();
    ssl_info_.reset();
    socket_.reset();
    if (io__)
    {
        if (!io_.stopped())
        {
            io_.stop();
        }
        delete io__;
    }
}

void tcp_pack::init(const bool use_ssl)
{
    flag_ssl_ = use_ssl;
    if (flag_ssl_)
    {
        ssl_socket_.reset(new ssl_socket(io_, ssl_info_->get_context()));
    }
    else
    {
        socket_.reset(new tcp_socket(io_));
    }
    port_ = 0;
}


bool tcp_pack::use_ssl() const
{
    return flag_ssl_;
}

boost::system::error_code tcp_pack::set_remote_address_and_port()
{
    boost::system::error_code ec;
    network::endpoint ep;
    if (use_ssl())
    {
        ep = ssl_socket_->lowest_layer().remote_endpoint(ec);
    }
    else
    {
        ep = socket_->remote_endpoint(ec);
    }
    if (ec)
    {
        return ec;
    }
    address_ = ep.address().to_string();
    port_ = ep.port();
    return ec;
}

std::string tcp_pack::get_local_address() const {
	boost::system::error_code ec;
	network::endpoint ep;
	if (use_ssl())
	{
		ep = ssl_socket_->lowest_layer().local_endpoint(ec);
	}
	else
	{
		ep = socket_->local_endpoint(ec);
	}
	if (ec)
	{
		return std::string();
	}
	return ep.address().to_string();
}

uint16_t tcp_pack::get_local_port() const {
	boost::system::error_code ec;
	network::endpoint ep;
	if (use_ssl())
	{
		ep = ssl_socket_->lowest_layer().local_endpoint(ec);
	}
	else
	{
		ep = socket_->local_endpoint(ec);
	}
	if (ec)
	{
		return 0;
	}

	return ep.port();
}

std::string tcp_pack::get_remote_address() const
{
    return address_;
}

uint16_t tcp_pack::get_remote_port() const
{
    return port_;
}

io_context& tcp_pack::get_io_context()
{
    return io_;
}

ssl_info* tcp_pack::get_ssl_info() const
{
    return ssl_info_.get();
}

ssl_socket* tcp_pack::get_ssl_socket()
{
    return ssl_socket_.get();
}

tcp_socket* tcp_pack::get_socket()
{
    return socket_.get();
}


} // namespace network


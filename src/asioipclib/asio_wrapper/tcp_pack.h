//
// Created by realbro on 19. 6. 24.
//

#ifndef UTIL_TCP_PACK_H
#define UTIL_TCP_PACK_H

#include "typedef.h"
#include <boost/shared_ptr.hpp>
#include "ssl_info.h"

namespace network {


class tcp_pack
{
public:
    tcp_pack();
    tcp_pack(io_context& io);
    tcp_pack(const ssl_method m);
    tcp_pack(io_context& io, const ssl_method m);
    tcp_pack(io_context& io, ssl_context& context);
    virtual ~tcp_pack();

private:
    void init(const bool use_ssl = false);

public:
    bool use_ssl() const;
    boost::system::error_code set_remote_address_and_port();
    std::string get_local_address() const;
    uint16_t get_local_port() const;
    std::string get_remote_address() const;
    uint16_t get_remote_port() const;
    io_context& get_io_context();
    ssl_info* get_ssl_info() const;
    ssl_socket* get_ssl_socket();
    tcp_socket* get_socket();

private:
    bool flag_ssl_;
    io_context* io__;
    io_context& io_;
    boost::shared_ptr<ssl_info> ssl_info_;
    boost::shared_ptr<ssl_socket> ssl_socket_;
    boost::shared_ptr<tcp_socket> socket_;
    std::string address_;
    uint16_t port_;
};

} // namespace network

#endif //UTIL_TCP_PACK_H

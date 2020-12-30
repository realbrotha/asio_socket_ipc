//
// Created by realbro on 19. 6. 19.
//

#ifndef UTIL_NETWORK_TYPEDEF_H
#define UTIL_NETWORK_TYPEDEF_H


#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace network {
typedef boost::asio::deadline_timer timer;
typedef boost::asio::ip::tcp::endpoint endpoint;
typedef boost::asio::ip::tcp::resolver resolver;
typedef boost::asio::ip::tcp::resolver::query query;
typedef boost::asio::ip::tcp::acceptor acceptor;
typedef boost::asio::io_context io_context;
typedef boost::asio::ip::tcp::socket tcp_socket;
typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;
typedef boost::asio::ssl::stream_base ssl_stream_base;
typedef boost::asio::ssl::context_base::method ssl_method;
typedef boost::asio::ssl::context ssl_context;
typedef boost::asio::basic_socket<boost::asio::ip::tcp> ssl_lowest_socket;


} // namespace network
#endif //UTIL_NETWORK_TYPEDEF_H

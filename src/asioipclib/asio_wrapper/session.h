//
// Created by realbro on 19. 6. 24.
//

#ifndef UTIL_NETWORK_SESSION_H
#define UTIL_NETWORK_SESSION_H

#include <deque>
#include <time.h>
#include <boost/thread/mutex.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include "typedef.h"
#include "buffer.h"
#include "tcp_pack.h"

namespace network {

class session
{
public:
    typedef boost::function<void (session*, boost::system::error_code, const size_t)> io_callback;
    typedef boost::function<void (session*, boost::system::error_code)> callback;
    session();
    session(io_context& io);
    session(const ssl_method m);
    session(io_context& io, const ssl_method m);
    session(io_context& io, ssl_context& context);
    virtual ~session();

    boost::system::error_code run();

    // option
    io_context& get_io_context();
    ssl_socket* get_ssl_socket();
    tcp_socket* get_socket();

    void set_id(const uint32_t id);
    uint32_t get_id() const;

    boost::system::error_code update_remote_address();
	std::string get_local_address() const;
	uint16_t get_local_port() const;
    std::string get_remote_address() const;
    uint16_t get_remote_port() const;

    buffer& get_read_buffer();

    void service_stop_force();
    void close_force();

    time_t get_lastest_event_time() const;
    time_t get_lastest_wrote_time() const;
    time_t get_lastest_read_time() const;

    void set_user_data(void* ptr);
    void* get_user_data() const;
    void reset_user_data(void* ptr = NULL);

    // async
    boost::system::error_code async_connect(const std::string& host, const std::string& service);
    void async_read(const size_t size, const size_t offset = 0);
    void async_read(uint8_t* buff, const size_t size);
    void async_write(const uint8_t* buff, const size_t size);
    void async_write(boost::shared_ptr<buffer>& buff);
    void async_close();
    virtual void async_connect_callback(const ssl_stream_base::handshake_type handshake_type, boost::system::error_code ec);
    virtual void async_read_callback(boost::system::error_code ec, const size_t try_size, const size_t size);
    virtual void async_read_callback(uint8_t* buff, boost::system::error_code ec, const size_t try_size, const size_t size);
    virtual void async_write_callback(boost::shared_ptr<buffer> write_buffer, const size_t try_size, const size_t size, boost::system::error_code ec);

    // ssl handler
    void async_handshake(const ssl_stream_base::handshake_type type = ssl_stream_base::server);
    void async_shutdown();
    void async_handshake_callback(boost::system::error_code ec);
    void async_shutdown_callback(boost::system::error_code ec);

    // callback setter
    void set_async_start_callback(callback callback);
    void set_async_read_callback(io_callback callback);
    void set_async_write_callback(io_callback callback);
    void set_async_close_callback(callback callback);


private:
    void init();

    // variables
private:
    boost::system::error_code asio_ec_;

    tcp_pack socket_;
    buffer rbuffer_;
    std::deque<boost::shared_ptr<buffer> > write_queue_list_;
    boost::mutex write_queue_mutex_;


    io_callback async_read_callback_;
    io_callback async_write_callback_;


    callback async_start_callback_;
    callback async_close_callback_;


    time_t lastest_event_time_;
    time_t lastest_wrote_time_;
    time_t lastest_read_time_;
    uint32_t id_;
    void* user_data_;
};

}


#endif //UTIL_NETWORK_SESSION_H

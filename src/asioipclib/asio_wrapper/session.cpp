//
// Created by realbro on 19. 6. 24.
//

#include "session.h"
#include "lib/util/defines/glog_wrapper.h"
#include <boost/bind.hpp>
#include <boost/thread/lock_guard.hpp>
#include <sstream>
#include <iomanip>

using namespace boost::system;

#ifndef DD
#define DD     std::cout<<__FILE__<<__LINE__<<std::endl
#endif

namespace network {

// 생성자에서 io_context 파라미터로 받는 경우에 synchornous service 는 사용을 추천 하지 않는다.
// 이 클래스에 작성된 synchronous service code 는 asynchronous service 기반으로 작성이 되었다.

session::session()
{
    init();
}

session::session(io_context& io)
: socket_(io)
{
    init();
}

session::session(const ssl_method m)
: socket_(m)
{
    init();
}

session::session(io_context& io, const ssl_method m)
: socket_(io, m)
{
    init();
}

session::session(io_context& io, ssl_context& context)
: socket_(io, context)
{
    init();
}

session::~session()
{}

void session::init()
{
    lastest_event_time_ = 0;
    lastest_wrote_time_ = 0;
    lastest_read_time_ = 0;
    id_ = 0;
}

// option
io_context& session::get_io_context()
{
    return socket_.get_io_context();
}

ssl_socket* session::get_ssl_socket()
{
    return socket_.get_ssl_socket();
}

tcp_socket* session::get_socket()
{
    return socket_.get_socket();
}

void session::set_id(const uint32_t id)
{
    id_ = id;
}

uint32_t session::get_id() const
{
    return id_;
}

std::string session::get_local_address() const {
	return socket_.get_local_address();
}

uint16_t session::get_local_port() const {
	return socket_.get_local_port();
}

std::string session::get_remote_address() const
{
    return socket_.get_remote_address();
}

uint16_t session::get_remote_port() const
{
    return socket_.get_remote_port();
}

buffer& session::get_read_buffer()
{
    return rbuffer_;
}

void session::service_stop_force()
{
    get_io_context().stop();
}

void session::close_force()
{
    boost::system::error_code ec;
    if (socket_.use_ssl())
    {
        socket_.get_ssl_socket()->lowest_layer().shutdown(boost::asio::socket_base::shutdown_both, ec);
        if (! ec || ec == boost::asio::error::eof) {
          ;
        } else {
          std::cout << "error in ssl shutdown " << ec.message();
        }
        socket_.get_ssl_socket()->lowest_layer().close(ec);
        if (! ec || ec == boost::asio::error::eof) {
          ;
        } else {
          std::cout << "error in ssl close " << ec.message();
        }
    }
    else
    {
        socket_.get_socket()->shutdown(boost::asio::socket_base::shutdown_both, ec);
        if (! ec || ec == boost::asio::error::eof) {
          ;
        } else {
          std::cout << "error in shutdown";
        }
        socket_.get_socket()->close(ec);
        if (! ec || ec == boost::asio::error::eof) {
          ;
        } else {
          std::cout << "error in close";
        }
    }
    if (ec)
    {
        ec_logging(ERROR, ec);
    }
}

time_t session::get_lastest_event_time() const
{
    return lastest_event_time_;
}

time_t session::get_lastest_wrote_time() const
{
    return lastest_wrote_time_;
}

time_t session::get_lastest_read_time() const
{
    return lastest_read_time_;
}

void session::set_user_data(void* ptr)
{
    user_data_ = ptr;
}

void* session::get_user_data() const
{
    return user_data_;
}

void session::reset_user_data(void* ptr)
{
    set_user_data(ptr);
}

void session::set_async_start_callback(callback callback)
{
    async_start_callback_ = callback;
}

void session::set_async_read_callback(io_callback callback)
{
    async_read_callback_ = callback;
}

void session::set_async_write_callback(io_callback callback)
{
    async_write_callback_ = callback;
}

void session::set_async_close_callback(callback callback)
{
    async_close_callback_ = callback;
}

// async
boost::system::error_code session::async_connect(const std::string& host, const std::string& service)
{
    try {
        resolver r(get_io_context());
        query q(host, service);
        resolver::iterator it = r.resolve(q);
        if (socket_.use_ssl()) {
            boost::asio::async_connect(socket_.get_ssl_socket()->lowest_layer(), it, boost::bind(&session::async_connect_callback, this, ssl_stream_base::client,boost::asio::placeholders::error));
        }
        else {
            boost::asio::async_connect(*socket_.get_socket(), it, boost::bind(&session::async_connect_callback, this, ssl_stream_base::client, boost::asio::placeholders::error));
        }
    } catch(const ::system_error &e)
    {
        return e.code();
    }
    return boost::system::error_code();
}

void session::async_read(const size_t size, const size_t offset)
{
    if (size == 0)
    {
        asio_ec_ = ::errc::make_error_code(::errc::invalid_argument);
        get_io_context().stop();
        return;
    }

    if (socket_.use_ssl())
    {
        socket_.get_ssl_socket()->async_read_some(boost::asio::buffer(rbuffer_.get(size + offset) + offset, size),
            boost::bind(&session::async_read_callback, this, boost::asio::placeholders::error, size, boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        socket_.get_socket()->async_read_some(boost::asio::buffer(rbuffer_.get(size + offset) + offset, size),
            boost::bind(&session::async_read_callback, this, boost::asio::placeholders::error, size, boost::asio::placeholders::bytes_transferred));
    }
}

void session::async_read(uint8_t* buff, const size_t size)
{
    if (size == 0)
    {
        asio_ec_ = ::errc::make_error_code(::errc::invalid_argument);
        get_io_context().stop();
        return;
    }

    if (socket_.use_ssl())
    {
        socket_.get_ssl_socket()->async_read_some(boost::asio::buffer(buff, size),
            boost::bind(&session::async_read_callback, this, buff, boost::asio::placeholders::error, size, boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        socket_.get_socket()->async_read_some(boost::asio::buffer(buff, size),
            boost::bind(&session::async_read_callback, this, buff, boost::asio::placeholders::error, size, boost::asio::placeholders::bytes_transferred));
    }
}


void session::async_write(const uint8_t* buff, const size_t size)
{
    if (buff == NULL || size == 0)
    {
        asio_ec_ = ::errc::make_error_code(::errc::invalid_argument);
        get_io_context().stop();
        return;
    }
    boost::shared_ptr<buffer> write_buffer(new buffer(buff, size));
    boost::lock_guard<boost::mutex> guard(write_queue_mutex_);
    if (write_queue_list_.size() == 0)
    {
        if (socket_.use_ssl())
        {
            socket_.get_ssl_socket()->async_write_some(boost::asio::buffer(write_buffer->get(), size),
                boost::bind(&session::async_write_callback, this, write_buffer->shared_from_this(), size, boost::asio::placeholders::bytes_transferred, boost::asio::placeholders::error));
        }
        else
        {
            socket_.get_socket()->async_write_some(boost::asio::buffer(write_buffer->get(), size),
                boost::bind(&session::async_write_callback, this, write_buffer->shared_from_this(), size, boost::asio::placeholders::bytes_transferred, boost::asio::placeholders::error));
        }
    }
    write_queue_list_.push_back(write_buffer);
}

void session::async_write(boost::shared_ptr<buffer>& buff)
{
    if (!buff || buff->size() == 0)
    {
        asio_ec_ = ::errc::make_error_code(::errc::invalid_argument);
        get_io_context().stop();
        return;
    }
    boost::lock_guard<boost::mutex> guard(write_queue_mutex_);
    if (write_queue_list_.size() == 0)
    {
        if (socket_.use_ssl())
        {
            socket_.get_ssl_socket()->async_write_some(boost::asio::buffer(buff->get(), buff->size()),
                                                       boost::bind(&session::async_write_callback, this, buff->shared_from_this(), buff->size(), boost::asio::placeholders::bytes_transferred, boost::asio::placeholders::error));
        }
        else
        {
            socket_.get_socket()->async_write_some(boost::asio::buffer(buff->get(), buff->size()),
                                                   boost::bind(&session::async_write_callback, this, buff->shared_from_this(), buff->size(), boost::asio::placeholders::bytes_transferred, boost::asio::placeholders::error));
        }
    }
    write_queue_list_.push_back(buff);
}

void session::async_close()
{
    if (socket_.use_ssl())
    {
        async_shutdown();
    }
    else
    {
        socket_.get_socket()->close(asio_ec_);
        if (async_close_callback_)
        {
            async_close_callback_(this, asio_ec_);
        }
    }
}

void session::async_connect_callback(const ssl_stream_base::handshake_type handshake_type, boost::system::error_code ec)
{
    lastest_event_time_ = lastest_wrote_time_ = lastest_read_time_ = time(NULL);
    socket_.set_remote_address_and_port();
    if (asio_ec_ == boost::asio::error::would_block) // synchronous handling
    {
        asio_ec_ = ec;
        return;
    }
    if (ec)
    {
        asio_ec_ = ec;
        async_start_callback_(this, ec);
        return;
    }
    if (socket_.use_ssl())
    {
        async_handshake(handshake_type);
        return;
    }
    if (async_start_callback_)
    {
        async_start_callback_(this, asio_ec_);
    }
}

void session::async_read_callback(boost::system::error_code ec, const size_t try_size, const size_t size)
{
    // TODO: 이 함수가 try_size 만큼 받아내지 못하여 몇번에 걸쳐 쪼개어 수신을 하면 asyn_read_callback_ 함수에 전달하는 size 값은
    // TODO: 총 수신 bytes-size 가 아니라 현재 콜백에서 받아낸 bytes-size 가 된다. 나중에 고쳐야함.
    if (!ec && try_size > size)
    {
        async_read(try_size - size, rbuffer_.size() - (try_size - size));
        return;
    }

    lastest_event_time_ = lastest_read_time_ = time(NULL);
    if (async_read_callback_)
    {
        async_read_callback_(this, ec, size);
    }
}

void session::async_read_callback(uint8_t* buff, boost::system::error_code ec, const size_t try_size, const size_t size)
{
    // TODO: 이 함수가 try_size 만큼 받아내지 못하여 몇번에 걸쳐 쪼개어 수신을 하면 asyn_read_callback_ 함수에 전달하는 size 값은
    // TODO: 총 수신 bytes-size 가 아니라 현재 콜백에서 받아낸 bytes-size 가 된다. 나중에 고쳐야함.
    if (!ec && try_size > size)
    {
        size_t left_size = try_size - size;
        async_read(buff + try_size - left_size, left_size);
        return;
    }

    lastest_event_time_ = lastest_read_time_ = time(NULL);
    if (async_read_callback_)
    {
        async_read_callback_(this, ec, size);
    }
}

std::string ToHexString(const uint8_t* buff, const size_t size)
{
    std::stringbuf buf;
    std::ostream s(&buf);
    for (size_t i=0; i<size; ++i)
    {
        int ssibal = static_cast<int>(buff[i]);
        s<<std::setfill('0')<<std::setw(2)<<std::hex<<ssibal;
        if (i+1 < size)
        {
            s<<" ";
        }
    }
    return buf.str();
}

void session::async_write_callback(boost::shared_ptr<buffer> write_buffer, const size_t try_size, const size_t size, boost::system::error_code ec)
{
    size_t left_size = try_size - size;
    uint8_t* buff = write_buffer->get();
    const size_t buff_size = write_buffer->size();
#if 0 // TEST CODE
    if (!ec)
    {
        if (try_size != size)
        {
            std::cout<<"wrote: "<<ToHexString(buff, size);
            std::cout<<"left: "<<ToHexString(buff + (write_buffer->size() - left_size), left_size);
        }
        else
        {
            std::cout<<" full: "<<ToHexString(buff, buff_size);
        }
    }
#endif
    if (!ec && size < try_size)
    {
        if (socket_.use_ssl())
        {
            socket_.get_ssl_socket()->async_write_some(boost::asio::buffer(write_buffer->get() + (write_buffer->size() - left_size), left_size),
                                                       boost::bind(&session::async_write_callback, this, write_buffer->shared_from_this(), left_size, boost::asio::placeholders::bytes_transferred, boost::asio::placeholders::error));
        }
        else
        {
            socket_.get_socket()->async_write_some(boost::asio::buffer(write_buffer->get() + (write_buffer->size() - left_size), left_size),
                                                   boost::bind(&session::async_write_callback, this, write_buffer->shared_from_this(), left_size, boost::asio::placeholders::bytes_transferred, boost::asio::placeholders::error));
        }
        return;
    }

    lastest_event_time_ = lastest_wrote_time_ = time(NULL);
    if (async_write_callback_)
    {
        async_write_callback_(this, ec, write_buffer->size() - left_size);
    }

    boost::lock_guard<boost::mutex> guard(write_queue_mutex_);
    write_queue_list_.pop_front();
    if (write_queue_list_.size() == 0)
    {
        return;
    }
    write_buffer = write_queue_list_.front();
    if (socket_.use_ssl())
    {
        socket_.get_ssl_socket()->async_write_some(boost::asio::buffer(write_buffer->get(), write_buffer->size()),
                                                   boost::bind(&session::async_write_callback, this, write_buffer->shared_from_this(), write_buffer->size(), boost::asio::placeholders::bytes_transferred, boost::asio::placeholders::error));
    }
    else
    {
        socket_.get_socket()->async_write_some(boost::asio::buffer(write_buffer->get(), write_buffer->size()),
                                               boost::bind(&session::async_write_callback, this, write_buffer->shared_from_this(), write_buffer->size(), boost::asio::placeholders::bytes_transferred, boost::asio::placeholders::error));
    }
}

void session::async_handshake(const ssl_stream_base::handshake_type type)
{
    socket_.get_ssl_socket()->async_handshake(type,
        boost::bind(&session::async_handshake_callback, this, boost::asio::placeholders::error));
}

void session::async_handshake_callback(boost::system::error_code ec)
{
    lastest_event_time_ = time(NULL);
    if (asio_ec_ == boost::asio::error::would_block)
    {
        asio_ec_ = ec;
        return;
    }
    asio_ec_ = ec;
    if (async_start_callback_)
    {
        async_start_callback_(this, asio_ec_);
    }
}

void session::async_shutdown()
{
    socket_.get_ssl_socket()->async_shutdown(boost::bind(&session::async_shutdown_callback, this, boost::asio::placeholders::error));
}

void session::async_shutdown_callback(boost::system::error_code ec)
{
    if (ec)
    {
        asio_ec_ = ec;
    }
    if (async_close_callback_)
    {
        async_close_callback_(this, ec);
    }
    socket_.get_ssl_socket()->lowest_layer().close(asio_ec_);
}

// io_context_wrapper
boost::system::error_code session::run()
{
    ::error_code ec;
    init();
    get_io_context().run(ec);
    get_io_context().reset();
    return ec;
}


} // namespace network


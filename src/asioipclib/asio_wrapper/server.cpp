//
// Created by realbro on 19. 6. 25.
//

#include "server.h"
#include <boost/bind.hpp>
#include "error_code.h"


#ifndef DD
#define DD     std::cout<<__FILE__<<__LINE__<<std::endl
#endif

using namespace boost::system;

namespace network {

server::server()
: io__(new io_context)
, io_(*io__)
, acceptor_(io_)
{
    init();
}
server::server(io_context& io)
: io__(NULL)
, io_(io)
, acceptor_(io_)
{
    init();
}
server::server(const ssl_method m)
: io__(new io_context)
, io_(*io__)
, acceptor_(io_)
, ssl_info_(new ssl_info(m))
{
    init();
}
server::server(io_context& io, const ssl_method m)
: io__(NULL)
, io_(io)
, acceptor_(io_)
, ssl_info_(new ssl_info(m))
{
    init();
}
server::server(io_context& io, ssl_context& context)
: io__(NULL)
, io_(io)
, acceptor_(io_)
, ssl_info_(new ssl_info(context))
{
    init();
}

server::~server()
{
    ssl_info_.reset();
    timer_.reset();
    if (new_session_)
    {
        delete new_session_;
    }
    if (io__)
    {
        if (!io_.stopped())
        {
            io_.stop();
        }
        delete io__;
    }
}

void server::init()
{
    new_session_ = NULL;
    async_procedure_interval_msec_ = 0;
}

boost::system::error_code server::get_lastest_error_code() const
{
    return ec_;
}

void server::refresh_session_list(uint32_t timeout_sec)
{
    time_t timeout = static_cast<time_t>(timeout_sec);
    time_t current = time(NULL);
    std::list<session*>::iterator it = session_list_.begin();
    for (; it != session_list_.end();)
    {
        bool erase = false;
        erase = !(*it)->get_ssl_socket()->lowest_layer().is_open();
        if (erase)
        {
            std::cout<<"sesison("<<(*it)->get_remote_address()<<":"<<(*it)->get_remote_port()<< ") closed."<<std::endl;
        }
        else if (!erase && timeout_sec > 0)
        {
            erase = (current - (*it)->get_lastest_event_time()) > timeout ? true : false;
            if (erase) {
                std::cout<<"sesison("<<(*it)->get_remote_address()<<":"<<(*it)->get_remote_port()<< ") timeout."<<std::endl;
            }
        }
        std::list<session*>::iterator tmp = it++;
        if (erase)
        {
            session* s = *tmp;
            session_list_.erase(tmp);
            delete s;
        }
    }
}

std::list<session*>& server::get_session_list()
{
    return session_list_;
}

ssl_info* server::get_ssl_info() const
{
    return ssl_info_.get();
}

void server::force_stop()
{
    io_.stop(); DD;
}

void server::set_session_start_callback(session::callback callback)
{
    session_start_callback_ = callback;
}

void server::set_session_read_callback(session::io_callback callback)
{
    session_read_callback_ = callback;
}

void server::set_session_write_callback(session::io_callback callback)
{
    session_write_callback_ = callback;
}

void server::set_session_close_callback(session::callback callback)
{
    session_close_callback_ = callback;
}

boost::system::error_code server::set_session_procedure_callback(session::callback callback, const uint32_t interval_msec)
{
    boost::system::error_code ec;
    if (interval_msec == 0 || !callback)
    {
        return ::errc::make_error_code(::errc::invalid_argument);
    }
    session_procedure_callback_interval_msec_ = interval_msec;
    session_procedure_callback_ = callback;
}

void server::set_session_callback(session* s)
{
    s->set_async_start_callback(session_start_callback_);
    s->set_async_read_callback(session_read_callback_);
    s->set_async_write_callback(session_write_callback_);
    s->set_async_close_callback(session_close_callback_);
    if (session_procedure_callback_ && session_procedure_callback_interval_msec_ > 0)
    {
//        s->set_async_procedure_callback(session_procedure_callback_, session_procedure_callback_interval_msec_);
    }
}

boost::system::error_code server::start(const std::string& host, const std::string& service)
{
    boost::system::error_code ec;
    ec_.clear();

    if (!session_start_callback_ || !session_read_callback_ || !session_write_callback_)
    {
        ec_ = set_error_code(network::error_code::callback_is_null);
        return ec_;
    }

    io_.reset();

    try
    {
        resolver r(io_);
        query q(host, service);
        resolver::iterator it = r.resolve(q);

        for (; it != resolver::iterator(); ++it)
        {
            acceptor_.open(it->endpoint().protocol(), ec);
            if (ec)
            {
                continue;
            }
            acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
            acceptor_.set_option(boost::asio::ip::tcp::acceptor::linger(true, 0));
            acceptor_.bind(*it);
            acceptor_.listen(boost::asio::socket_base::max_connections);
            break;
        }
        if (ec)
        {
            ec_ = ec;
            return ec_;
        }
    } catch(const boost::system::system_error& e)
    {
        // catch resolve, bind, listen error
        ec_ = e.code();
    }
    if (ec_)
    {
        return ec_;
    }
    if (async_procedure_interval_msec_ > 0 && async_procedure_callback_)
    {
        timer_.reset(new timer(io_));
        timer_->expires_from_now(boost::posix_time::milliseconds(async_procedure_interval_msec_), ec);
        if (ec)
        {
            ec_ = ec;
            return ec_;
        }
        timer_->async_wait(boost::bind(&server::async_procedure_callback, this, boost::asio::placeholders::error));
    }
    async_accept();
    io_.run(ec);

    if (ec)
    {
        // TODO: print error_code
        if (!ec_)
        {
            ec_ = ec;
        }
    }
    return ec_;
}

void server::async_accept()
{
    if (!ssl_info_)
    {
        new_session_ = new session(io_);
    } else
    {
        new_session_ = new session(io_, ssl_info_->get_context());
    }
    set_session_callback(new_session_);

    if (ssl_info_)
    {
        acceptor_.async_accept(new_session_->get_ssl_socket()->lowest_layer(),
            boost::bind(&server::async_accept_callback, this, boost::asio::placeholders::error));
    }
    else
    {
        acceptor_.async_accept(*new_session_->get_socket(),
            boost::bind(&server::async_accept_callback, this, boost::asio::placeholders::error));
    }
}

void server::async_accept_callback(const boost::system::error_code& ec)
{
    if (ec)
    {
        delete new_session_;
        new_session_ = NULL;
    }
    session* s = new_session_;
    s->async_connect_callback(ssl_stream_base::server, ec);
    session_list_.push_back(s);
    new_session_ = NULL;
    if (!ssl_info_)
    {
        new_session_ = new session(io_);
    } else
    {
        new_session_ = new session(io_, ssl_info_->get_context());
    }
    set_session_callback(new_session_);
    if (ssl_info_)
    {
        acceptor_.async_accept(new_session_->get_ssl_socket()->lowest_layer(),
            boost::bind(&server::async_accept_callback, this, boost::asio::placeholders::error));
    }
    else
    {
        acceptor_.async_accept(*new_session_->get_socket(),
            boost::bind(&server::async_accept_callback, this, boost::asio::placeholders::error));
    }
}

boost::system::error_code server::set_procedure_callback(callback callback, const uint32_t interval_msec)
{
    ec_.clear();
    if (interval_msec == 0 || !callback)
    {
        ec_ = ::errc::make_error_code(::errc::invalid_argument);
        return ec_;
    }
    async_procedure_interval_msec_ = interval_msec;
    async_procedure_callback_ = callback;
    return ec_;
}

void server::async_procedure_callback(const boost::system::error_code& ec)
{
    if (async_procedure_callback_)
    {
        async_procedure_callback_(this, ec);
    }
    if (ec)
    {
        ec_ = ec;
        io_.stop();
        return;
    }
    boost::system::error_code ec2;
    timer_->expires_from_now(boost::posix_time::milliseconds(async_procedure_interval_msec_), ec2);
    if (ec2)
    {
        ec_ = ec2;
        io_.stop();
        return;
    }
    timer_->async_wait(boost::bind(&server::async_procedure_callback, this, boost::asio::placeholders::error));
}
SessionManager& server::GetSessionManager() {
  return session_manager_;
}
} // namespace network


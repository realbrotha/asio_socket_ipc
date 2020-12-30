//
// Created by realbro on 19. 6. 25.
//

#ifndef UTIL_NETWORK_SERVER_H
#define UTIL_NETWORK_SERVER_H

#include "session.h"
#include "SessionManager.h"

#include <list>

namespace network {

class server {
 public:
  typedef boost::function<void(server *, const boost::system::error_code &)> callback;
  server();
  server(io_context &io);
  server(const ssl_method m);
  server(io_context &io, const ssl_method m);
  server(io_context &io, ssl_context &context);
  virtual ~server();

  void refresh_session_list(uint32_t timeout_sec = 0);
  std::list<session *> &get_session_list();
  ssl_info *get_ssl_info() const;
  void force_stop();
  boost::system::error_code start(const std::string &host, const std::string &service);

  // set session callback
  void set_session_start_callback(session::callback callback);
  void set_session_read_callback(session::io_callback callback);
  void set_session_write_callback(session::io_callback callback);
  void set_session_close_callback(session::callback callback);
  boost::system::error_code set_session_procedure_callback(session::callback callback, const uint32_t interval_msec);

  SessionManager &GetSessionManager();
 private:
  void init();
  void set_session_callback(session *s);
  void async_accept();
  void async_accept_callback(const boost::system::error_code &ec);

 private:
  boost::system::error_code ec_;
  io_context *io__;
  io_context &io_;

  acceptor acceptor_;
  boost::shared_ptr<ssl_info> ssl_info_;
  uint32_t async_procedure_interval_msec_;
  callback async_procedure_callback_;
  boost::shared_ptr<timer> timer_;
  std::list<session *> session_list_;

  // session
  session *new_session_;
  session::callback session_start_callback_;
  session::io_callback session_read_callback_;
  session::io_callback session_write_callback_;
  session::callback session_close_callback_;
};

} // namespace network
#endif //UTIL_SERVER_H

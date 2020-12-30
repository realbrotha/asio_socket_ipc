//
// Created by realbro on 12/28/20.
//

#ifndef INTEGRATION_AGENT_SRC_LIB_COMMONEX_UNIXDOMAINSOCKET_SOCKETIPCSERVER_H_
#define INTEGRATION_AGENT_SRC_LIB_COMMONEX_UNIXDOMAINSOCKET_SOCKETIPCSERVER_H_

#include "MessageManager.h"
#include "MessageDefine.hpp"
#include "SocketFactoryBase.h"

#include "asio_wrapper/server.h"
#include "asio_wrapper/session.h"

#include "boost/thread.hpp"
#include "boost/bind.hpp"
#include "boost/thread/scoped_thread.hpp"

namespace {
typedef boost::function<void(boost::shared_ptr<IpcJsonMessage> message)> t_Callback;
typedef boost::scoped_thread<boost::join_if_joinable, boost::thread> scope_thread;
}

class SocketIpcServer : public SocketFactoryBase {
 public:
  SocketIpcServer();
  virtual ~SocketIpcServer();

  bool Initialize(int16_t product_code,
                  t_Callback connect_callback,
                  t_Callback disconnect_callback,
                  t_Callback read_callback);
  void Finalize();
  bool SendSync(std::string &send_string,
                int16_t msg_type,
                boost::shared_ptr<IpcJsonMessage> &out_buffer,
                boost::chrono::milliseconds timeout_msec,
                int16_t dst,
                int32_t seq_id = 0);
  bool SendASync(std::string &send_string, int16_t msg_type, int16_t dst, int32_t seq_id = 0);
 private:
  void StartThread();
  void StopThread();
  void ThreadWorker(); // thread impl

  void SetCallbackProcedure();

  MessageManager message_manager_;
  scope_thread worker_thread_;
  int16_t product_code_;
  int seq_id_; // TODO : 바꿔야함

  t_Callback connect_callback_proc_;
  t_Callback disconnect_callback_proc_;
  t_Callback read_callback_proc_;

  void server_start_callback(network::session *s, const boost::system::error_code &ec);
  void server_read_callback(network::session *s, const boost::system::error_code &ec, const size_t size);
  void server_write_callback(network::session *s, const boost::system::error_code &ec, const size_t size);
  void server_close_callback(network::session *s, const boost::system::error_code &ec);

  boost::shared_ptr<network::server> server_;
};

#endif //INTEGRATION_AGENT_SRC_LIB_COMMONEX_UNIXDOMAINSOCKET_SOCKETIPCSERVER_H_

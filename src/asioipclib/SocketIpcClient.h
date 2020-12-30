//
// Created by realbro on 12/22/20.
//

#ifndef INTEGRATION_AGENT_SRC_LIB_COMMONEX_UNIXDOMAINSOCKET_SOCKETIPCCLIENT_H_
#define INTEGRATION_AGENT_SRC_LIB_COMMONEX_UNIXDOMAINSOCKET_SOCKETIPCCLIENT_H_

#include "SocketFactoryBase.h"
#include "MessageDefine.hpp"
#include "MessageManager.h"

#include "asio_wrapper/session.h"
#include "asio_wrapper/typedef.h"
//#include "asio_wrapper/error_code.h"

#include "boost/thread.hpp"
#include "boost/bind.hpp"
#include "boost/thread/scoped_thread.hpp"

namespace {

typedef boost::function<void(boost::shared_ptr<IpcJsonMessage> message)> t_Callback;
typedef boost::scoped_thread<boost::join_if_joinable, boost::thread> scope_thread;

}
class SocketIpcClient : public SocketFactoryBase {

 public:
  SocketIpcClient();
  virtual ~SocketIpcClient();

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

 private :
  void StartThread();
  void StopThread();
  void ThreadWorker(); // thread impl
  void SetCallbackProcedure();

  bool ErrorCheck(const boost::system::error_code &ec);

  bool SendServerHello();
  void session_start_callback(network::session *s, const boost::system::error_code &ec);
  void session_read_callback(network::session *s, const boost::system::error_code &ec, const size_t size);
  void session_write_callback(network::session *s, const boost::system::error_code &ec, const size_t size);
  void session_close_callback(network::session *s, const boost::system::error_code &ec);

  MessageManager message_manager_;
  scope_thread worker_thread_;
  int16_t product_code_;
  int seq_id_; // TODO : 바꿔야함

  boost::shared_ptr<network::session> session_;

  t_Callback connect_callback_proc_;
  t_Callback disconnect_callback_proc_;
  t_Callback read_callback_proc_;
};

#endif //INTEGRATION_AGENT_SRC_LIB_COMMONEX_UNIXDOMAINSOCKET_SOCKETIPCCLIENT_H_

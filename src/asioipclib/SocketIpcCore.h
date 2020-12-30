//
// Created by realbro on 20. 5. 26..
//

#ifndef TESTIPC_UNIXDOMAINSOCKET_SRC_UNIXDOMAINSOKET_LIB_UNIXDOMAINSOCKETCORE_H_
#define TESTIPC_UNIXDOMAINSOCKET_SRC_UNIXDOMAINSOKET_LIB_UNIXDOMAINSOCKETCORE_H_

#include "SocketFactoryBase.h"

#include "boost/move/unique_ptr.hpp"
//#include <thread>

class SocketIpcCore {
 public :
  SocketIpcCore();
  ~SocketIpcCore();

  bool Initialize(t_Callback ConnectCallback,
                  t_Callback DisconnectCallback,
                  t_Callback ReadCallback, int16_t product_code, bool server_mode = false);
  bool Finalize();

  bool SendASync(std::string data, int16_t msg_type, int16_t dst, int32_t seq_id = 0);
  bool SendSync(std::string data,
                int16_t msg_type,
                boost::shared_ptr<IpcJsonMessage>& out_buffer,
                boost::chrono::milliseconds timeout_msec,
                int16_t dst,
                int32_t seq_id = 0);
 private:

  boost::movelib::unique_ptr<SocketFactoryBase> soeket_core_;
};

#endif //TESTIPC_UNIXDOMAINSOCKET_SRC_UNIXDOMAINSOKET_LIB_UNIXDOMAINSOCKETCORE_H_

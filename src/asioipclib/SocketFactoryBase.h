//
// Created by realbro on 20. 5. 26..
//

#ifndef TESTIPC_UNIXDOMAINSOCKET_SRC_UNIXDOMAINSOKET_LIB_UNIXDOMAINSOCKETFACTORYBASE_H_
#define TESTIPC_UNIXDOMAINSOCKET_SRC_UNIXDOMAINSOKET_LIB_UNIXDOMAINSOCKETFACTORYBASE_H_

#define PURE 0

#include "MessageDefine.hpp"

#include <string>
#include <vector>
#include "boost/function.hpp"
#include "boost/chrono.hpp"

//typedef void (*t_ListenerCallbackProc)(std::array<char, 1024> message);
typedef boost::function<void(boost::shared_ptr<IpcJsonMessage> message)> t_Callback;

class SocketFactoryBase {
 public :
  SocketFactoryBase() {}
  virtual ~SocketFactoryBase() {}

  virtual bool SendSync(std::string &send_string,
                        int16_t msg_type,
                        boost::shared_ptr<IpcJsonMessage>& out_buffer,
                        boost::chrono::milliseconds timeout_msec,
                        int16_t dst,
                        int32_t seq_id = 0) = PURE;
  virtual bool SendASync(std::string &send_string, int16_t msg_type, int16_t dst, int32_t seq_id = 0) = PURE;
  virtual bool Initialize(int16_t product_code,
                          t_Callback ConnectCallback = NULL,
                          t_Callback DisconnectCallback = NULL,
                          t_Callback ReadCallback = NULL) = PURE;
  virtual void Finalize() = PURE;
};

#endif //TESTIPC_UNIXDOMAINSOCKET_SRC_UNIXDOMAINSOKET_LIB_UNIXDOMAINSOCKETFACTORYBASE_H_

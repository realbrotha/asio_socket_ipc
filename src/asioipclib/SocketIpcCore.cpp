//
// Created by realbro on 20. 5. 26..
//

#include "SocketIpcCore.h"
#include "SocketIpcClient.h"
#include "SocketIpcServer.h"

SocketIpcCore::SocketIpcCore() {

}
SocketIpcCore::~SocketIpcCore() {

}

bool SocketIpcCore::Initialize(t_Callback ConnectCallback,
                               t_Callback DisconnectCallback,
                               t_Callback ReadCallback, int16_t product_code, bool server_mode) {
  if (server_mode) {
    //printf ("Core::Init server\n\n");
    soeket_core_.reset(new SocketIpcServer());
  } else {
    //printf ("Core::Init client\n\n");
    soeket_core_.reset(new SocketIpcClient());
  }
  soeket_core_->Initialize(product_code, ConnectCallback, DisconnectCallback, ReadCallback);
}

bool SocketIpcCore::Finalize() {
  if (soeket_core_.get()) {
    printf ("Finaleize`12`1`1`1");
    soeket_core_->Finalize();
  }
  soeket_core_.reset();

  return false;
}

bool SocketIpcCore::SendASync(std::string data, int16_t msg_type, int16_t dst, int32_t seq_id) {
  bool result = false;
  std::cout << "SocketIpcCore STart";
  if (soeket_core_.get()) {
    //socket_core_->SyncSend1(data,dst);
    std::cout << "SocketIpcCore STart - asyn called";
    result = soeket_core_->SendASync(data, msg_type, dst, seq_id);
  }
  std::cout << "SocketIpcCore STart - async ";
  return result;
}

bool SocketIpcCore::SendSync(std::string data,
                             int16_t msg_type,
                             boost::shared_ptr<IpcJsonMessage>& out_buffer,
                             boost::chrono::milliseconds timeout_msec,
                             int16_t dst,
                             int32_t seq_id) {
  bool result = false;
  if (soeket_core_.get()) {
    //socket_core_->SyncSend1(data,dst);
    result = soeket_core_->SendSync(data, msg_type, out_buffer, timeout_msec, dst, seq_id);
    std::cout <<"[Core] Send Sync done" << result;
  }
  std::cout <<"[Core] Send Sync done : " << result;
  return result;
}

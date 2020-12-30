//
// Created by realbro on 12/22/20.
//

#include "SocketIpcClient.h"
#include "MessageDefine.hpp"

SocketIpcClient::SocketIpcClient() : product_code_(0) {

}
SocketIpcClient::~SocketIpcClient() {
  Finalize();
}
bool SocketIpcClient::Initialize(int16_t product_code,
                                 t_Callback connect_callback,
                                 t_Callback disconnect_callback,
                                 t_Callback read_callback) {
  product_code_ = product_code;

  if (connect_callback)
    connect_callback_proc_ = boost::move(connect_callback);
  if (disconnect_callback)
    disconnect_callback_proc_ = boost::move(disconnect_callback);
  if (read_callback)
    read_callback_proc_ = boost::move(read_callback);

  session_.reset(new network::session());

  SetCallbackProcedure();
  StartThread();
  return true;
}

void SocketIpcClient::StartThread() {

  try {
    boost::thread th(BOOST_THREAD_MAKE_RV_REF(boost::thread(&SocketIpcClient::ThreadWorker, boost::ref(*this))));
    worker_thread_ = BOOST_THREAD_MAKE_RV_REF(scope_thread(boost::move(th)));
  } catch (const boost::thread_resource_error &ex) {
    std::cout << "SocketIpcCLient StartThread failed. " << ex.what();
    return;
  }
}

void SocketIpcClient::Finalize() {

  if (!!session_) {
    session_->close_force();
    session_->service_stop_force();
    session_.reset();
  }
  StopThread();
}

void SocketIpcClient::StopThread() {
  if (!worker_thread_.joinable()) {
    return;
  }
  if (worker_thread_.get_id() == boost::this_thread::get_id()) {
    worker_thread_.detach();
    return;
  }
  try {
    worker_thread_.join();
  } catch (boost::thread_exception &ex) {
    std::cout << " boost::thread_exception thrown !! " << ex.what();
  } catch (...) {
    std::cout << " std::exception else thrown !! ";
  }
}

void SocketIpcClient::ThreadWorker() {
  std::cout << "ThreadWorker ";

  const std::string tango_ip = "127.0.0.1";
  const std::string tango_port = "8888";
  std::cout << "ThreadWorker - async_connect ";
  if (!session_->async_connect(tango_ip, tango_port)) {
    std::cout << "ThreadWorker - run";
    session_->run(); //wating for close eventg
  }
  std::cout << "ThreadWorker - stop";

}

bool SocketIpcClient::SendSync(std::string &send_string,
                               int16_t msg_type,
                               boost::shared_ptr<IpcJsonMessage>& out_buffer,
                               boost::chrono::milliseconds timeout_msec,
                               int16_t dst,
                               int32_t seq_id) {
  std::cout << "Client Send Sync!!!";
  IpcJsonMessage msg;
  msg.listener_id = msg_type;
  msg.sequence_id = seq_id ? seq_id : ++seq_id_;
  msg.src = product_code_;
  msg.dst = dst;
  msg.synchronous = true;
  msg.data_size = send_string.length();
  msg.data_string = send_string;

  std::vector<char> serialize;
  msg.Serialize(serialize);

  session_->async_write(reinterpret_cast<uint8_t *>(&serialize[0]), serialize.size());
  std::cout << "Message ADD!!! : Message ID" << msg.sequence_id;
  if (message_manager_.Add(msg.sequence_id, msg.listener_id, msg.src, timeout_msec)) {
    std::cout << "Message get data!!!";
    out_buffer = message_manager_.GetData(msg.sequence_id, msg.listener_id);
  }

  return (!!out_buffer) ? true : false;
}

bool SocketIpcClient::SendASync(std::string &send_string, int16_t msg_type, int16_t dst, int32_t seq_id) {
  std::cout <<" SocketIpcClient!~!~!~!~1 ASYnc";
  IpcJsonMessage msg;
  msg.listener_id = msg_type;
  msg.sequence_id = seq_id ? seq_id_ : seq_id_++;
  msg.src = product_code_;
  msg.dst = dst;
  msg.synchronous = false;
  msg.data_size = send_string.length();
  msg.data_string = send_string;

  std::vector<char> serialize;
  msg.Serialize(serialize);

  session_->async_write(reinterpret_cast<uint8_t *>(&serialize[0]), serialize.size());

  return true;
}

void SocketIpcClient::session_start_callback(network::session *s, const boost::system::error_code &ec) {
  if (ec) {
    std::cout << "[session_start_callback] ERROR: " << ec.category().name() << ", meesage: " << ec.message();
    sleep(2);
    //StopThread();
    std::string tango_ip = "127.0.0.1";
    std::string tango_port = "8888";

    session_->async_connect(tango_ip, tango_port);
    return;
  }
  std::cout << "Start Session STart";
  if (connect_callback_proc_) {
    boost::shared_ptr<IpcJsonMessage> message;
    connect_callback_proc_(message);
  }
  //SendServerHello();
  IpcJsonMessage msg_;
  s->async_read(msg_.GetHeaderSize());
}
bool SocketIpcClient::SendServerHello() {
  std::cout << " SendServer Hello";
  IpcJsonMessage msg;
  msg.listener_id = MESSAGE_TYPE::HELLO;
  msg.sequence_id = 0;
  msg.src = product_code_;
  msg.dst = PRODUCT_TYPE::INTEGRATION_AGENT;
  msg.synchronous = true;
  msg.data_string = "";

  std::vector<char> serialize;
  msg.Serialize(serialize);

  session_->async_write(reinterpret_cast<uint8_t *>(&serialize[0]), serialize.size());

  return true;
}
void SocketIpcClient::session_read_callback(network::session *s,
                                            const boost::system::error_code &ec,
                                            const size_t size) {
  if (ErrorCheck(ec)) return;

  network::buffer &buff = s->get_read_buffer();
  IpcJsonMessage msg_;
  if (buff.size() == msg_.GetHeaderSize()) {
    if (!msg_.DeserializeHeader(buff.get(), buff.size())) {
      buff.reset();
      s->async_read(msg_.GetHeaderSize());
      return;
    }
    if (msg_.data_size > 0) {
      s->async_read(msg_.data_size, msg_.GetHeaderSize());
      return;
    }
  }
  std::vector<char> buffer(buff.get(), buff.get() + buff.size());

  boost::shared_ptr<IpcJsonMessage> message(new IpcJsonMessage());
  message->Deserialize(buffer);
  std::cout <<"Message id :" <<message->sequence_id;
  std::cout <<"Message data :" <<message->data_string;
  if (message_manager_.IsRegisteredMessage(message->sequence_id, message->listener_id)) { // NOTE : fake sync
    std::cout << "is registered message";
    if (message_manager_.IsSyncMessage(message->sequence_id, message->listener_id)) {
      std::cout <<" IS SYNC MEssage!!!";
      message_manager_.SetData(message->sequence_id, message->listener_id, message);
    } else {
      std::cout <<" IS ASYNC MEssage!!!";
      SendASync(message->data_string, message->listener_id, message->dst, message->sequence_id);
    }
  } else {
    std::cout << "callback";
    if (read_callback_proc_) {
      read_callback_proc_(message);
    }
  }
//  boost::asio::post(receive_worker_pool_, boost::bind(read_callback_proc_, this, message->shared_from_this()));
  s->async_read(msg_.GetHeaderSize());
}

void SocketIpcClient::session_write_callback(network::session *s,
                                             const boost::system::error_code &ec,
                                             const size_t size) {
  printf ("Socket Wirte callback\n");
  if (ErrorCheck(ec)) return;
}

void SocketIpcClient::session_close_callback(network::session *s, const boost::system::error_code &ec) {
  printf("CLOSE!!!!!!!!!!!!!!!");
}

void SocketIpcClient::SetCallbackProcedure() {
  session_->set_async_start_callback(boost::bind(&SocketIpcClient::session_start_callback, this, _1, _2));
  session_->set_async_read_callback(boost::bind(&SocketIpcClient::session_read_callback, this, _1, _2, _3));
  session_->set_async_write_callback(boost::bind(&SocketIpcClient::session_write_callback, this, _1, _2, _3));
  session_->set_async_close_callback(boost::bind(&SocketIpcClient::session_close_callback, this, _1, _2));
  return;
}

bool SocketIpcClient::ErrorCheck(const boost::system::error_code &ec) {
  bool stop_flag = false;
  if (ec) {
    std::cout<< "[ErrorCheck] ERROR: " << ec.category().name() << ", " << ec.message() << "value : " << ec.value();

    StopThread();
    StartThread();
    stop_flag = true;
  }

  return stop_flag;
}
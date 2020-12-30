//
// Created by realbro on 12/28/20.
//

#include "SocketIpcServer.h"

#include "boost/bind.hpp"
#include "MessageDefine.hpp"

void SocketIpcServer::server_start_callback(network::session *s, const boost::system::error_code &ec) {
  printf ("Start!!!\n");
  IpcJsonMessage msg;
  printf ("read Start!!!\n");
  s->async_read(msg.GetHeaderSize());
  printf ("read END~!!!!\n");
}
void SocketIpcServer::server_close_callback(network::session *s, const boost::system::error_code &ec) {
  printf ("Close!!!\n");
}

void SocketIpcServer::server_read_callback(network::session *s,
                                            const boost::system::error_code &ec,
                                            const size_t size) {
  printf ("read f??");
  std::cout << "[Asio SERver]Read!!!";

  if (ec) {
    std::cout << "session_read_callback ERROR : " << ec.category().name() << ", " << ec.message();
    //boost::lock_guard<boost::mutex> lock_guard(error_code_mutex_);
    //std::cout <<" Remove List result : " << session_manager_.Remove(s);;
    IpcJsonMessage message;
    s->async_read(message.GetHeaderSize());
    server_;
    //server_->refresh_session_list();
    return;
    if (ec != boost::asio::error::operation_aborted) {
      //ec_ = ec;
      //Stop();
    }
    //return;
  }

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
  if (message->listener_id == MESSAGE_TYPE::HELLO) {
    std::cout << "hello ~!~! client : " << message->src;
    //s->async_close();
    server_->GetSessionManager().AddCompleteSession(message->src, s);
    //session_manager_.Add(message2->src, s);
  }

  if (message_manager_.IsRegisteredMessage(message->sequence_id, message->listener_id)) { // NOTE : fake sync
    std::cout << "is registered message";
    if (message_manager_.IsSyncMessage(message->sequence_id, message->listener_id)) {
      std::cout <<" IS SYNC MEssage!!!";
      message_manager_.SetData(message->sequence_id, message->listener_id, message);
    } else {
      std::cout <<" IS ASYNC MEssage!!!";
      //SendASync(message->data_string, message->listener_id, message->dst, message->sequence_id);
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
void SocketIpcServer::server_write_callback(network::session *s,
                                             const boost::system::error_code &ec,
                                             const size_t size)
{
  printf ("Write!!!\n");
}
SocketIpcServer::SocketIpcServer() {

}
SocketIpcServer::~SocketIpcServer() {
  std::cout <<"BOMB~!";
  Finalize();
}

bool SocketIpcServer::Initialize(int16_t product_code,
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

  server_.reset(new network::server());

  SetCallbackProcedure();
  StartThread();
  return true;
}
void SocketIpcServer::Finalize() {
  std::cout <<"SocketIpcServer Finalize";

  if (!!server_) {
    std::cout <<"SocketIpcServer reset?";
    server_->force_stop();
    server_.reset();
  }
  StopThread();
}

void SocketIpcServer::SetCallbackProcedure() {
  // set session callback

  server_->set_session_start_callback(boost::bind(&SocketIpcServer::server_start_callback, this, _1, _2));
  server_->set_session_read_callback(boost::bind(&SocketIpcServer::server_read_callback, this, _1, _2, _3));
  server_->set_session_write_callback(boost::bind(&SocketIpcServer::server_write_callback, this, _1, _2, _3));
  server_->set_session_close_callback(boost::bind(&SocketIpcServer::server_close_callback, this, _1, _2));
  return;
}

void SocketIpcServer::ThreadWorker() {
  std::cout << "ThreadWorker ";


  const std::string tango_ip = "127.0.0.1";
  const std::string tango_port = "8888";
  std::cout << "ThreadWorker - server start ";
  if (!server_->start(tango_ip, tango_port)) {
    std::cout << "ThreadWorker - run";
    //server_->run(); //wating for close eventg
  }
  std::cout << "ThreadWorker - done";
}

void SocketIpcServer::StartThread() {

  std::cout << "Start Thread";

  try {
    boost::thread th(BOOST_THREAD_MAKE_RV_REF(boost::thread(&SocketIpcServer::ThreadWorker, boost::ref(*this))));
    worker_thread_ = BOOST_THREAD_MAKE_RV_REF(scope_thread(boost::move(th)));
  } catch (const boost::thread_resource_error &ex) {
    std::cout << "SocketIpcCLient StartThread failed. " << ex.what();
    return;
  }
}

void SocketIpcServer::StopThread() {
  std::cout << "StopThread-1";
  if (!worker_thread_.joinable()) {
    std::cout << "StopThread-2";
    return;
  }
  if (worker_thread_.get_id() == boost::this_thread::get_id()) {
    worker_thread_.detach();
    std::cout << "StopThread-3";
    return;
  }
  try {
    std::cout << "StopThread-4";
    worker_thread_.join();
  } catch (boost::thread_exception &ex) {
    std::cout << " boost::thread_exception thrown !! " << ex.what();
  } catch (...) {
    std::cout << " std::exception else thrown !! ";
  }
}
bool SocketIpcServer::SendSync(std::string &send_string,
                               int16_t msg_type,
                               boost::shared_ptr<IpcJsonMessage> &out_buffer,
                               boost::chrono::milliseconds timeout_msec,
                               int16_t dst,
                               int32_t seq_id) {
  // hack : not yet;
  return false;
}
bool SocketIpcServer::SendASync(std::string &send_string, int16_t msg_type, int16_t dst, int32_t seq_id) {
  // hack : not yet;
  return false;
}

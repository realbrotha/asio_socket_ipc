//
// Created by realbro on 12/14/20.
//
#include "EzTimer.h"
#include "MessageManager.h"
#include "boost/thread/mutex.hpp"

#include "glog/logging.h"
MessageManager::MessageManager() : read_data_(true) {
  Initialize();
}

MessageManager::~MessageManager() {
  ez_timer_.Stop();
}

void MessageManager::TimerOutCallback() {
  ez_timer_.Stop();
  Remove(wait_sync_id_.first, wait_sync_id_.second);
  read_data_ = false; //읽지 말것
}

bool MessageManager::Initialize() {
  ez_timer_.Initialize(boost::bind(&MessageManager::TimerOutCallback, this), true, true);
}

bool MessageManager::Add(int message_id, int listener_id, int src, boost::chrono::milliseconds msec) {
  std::cout <<" message ADD!!!";

  ez_timer_.SetInterval(msec);
  {
    boost::unique_lock<boost::mutex> lock(mtx_);
    if (!sync_message_list.count(std::pair<int, int>(message_id, listener_id))) {
      struct ExtendedInfo buffer;
      buffer.is_sync = true;
      buffer.src_socket_fd = src;
      //boost::shared_ptr<IpcJsonMessage> buffer;
      //sync_message_list[message_id] = buffer;
      sync_message_list.insert(std::make_pair(std::make_pair(message_id, listener_id), buffer));
      wait_sync_id_ = std::make_pair(message_id, listener_id);
    }
  }
  std::cout <<" message timer start!!!";
  ez_timer_.SetInterval(msec);
  ez_timer_.Start();

  return read_data_;
}

bool MessageManager::Add(int message_id, int listener_id, int src) {
  std::cout <<" message add for async!!!";

  {
    boost::unique_lock<boost::mutex> lock(mtx_);
    if (!sync_message_list.count(std::pair<int, int>(message_id, listener_id))) {
      struct ExtendedInfo buffer;
      buffer.is_sync = false;
      buffer.src_socket_fd = src;
      //boost::shared_ptr<IpcJsonMessage> buffer;
      //sync_message_list[message_id] = buffer;
      sync_message_list.insert(std::make_pair(std::make_pair(message_id, listener_id), buffer));
      wait_sync_id_ = std::make_pair(message_id, listener_id);
    }
  }
  std::cout <<" message timer start!!!";

  return read_data_;
}
bool MessageManager::Remove(int message_id, int listener_id) {
  bool result = false;
  std::map<std::pair<int,int>, ExtendedInfo >::iterator it;
  boost::unique_lock<boost::mutex> lock(mtx_);
  for (it = sync_message_list.begin(); it != sync_message_list.end(); ++it) {
    if (it->first.first == message_id && it->first.second == listener_id) {
      sync_message_list.erase(it);
      result = true;
      break;
    }
  }
  return result;
}

bool MessageManager::IsRegisteredMessage(int message_id, int listener_id) {
  boost::unique_lock<boost::mutex> lock(mtx_);
  return sync_message_list.count(std::make_pair(message_id, listener_id));
}

int MessageManager::IsSyncMessage(int message_id, int listener_id) {
  bool result = -1;
  std::map<std::pair<int,int>, ExtendedInfo >::iterator it;
  boost::unique_lock<boost::mutex> lock(mtx_);
  for (it = sync_message_list.begin(); it != sync_message_list.end(); ++it) {
    if (it->first.first == message_id && it->first.second == listener_id) {
      result = it->second.is_sync;
      break;
    }
  }
  return result;
}
bool MessageManager::SetData(int message_id, int listener_id, boost::shared_ptr<IpcJsonMessage>& data) {
  boost::unique_lock<boost::mutex> lock(mtx_);
  std::map<std::pair<int,int>, ExtendedInfo >::iterator it;
  for (it = sync_message_list.begin(); it != sync_message_list.end(); ++it) {
    if (it->first.first == message_id && it->first.second == listener_id) {
      it->second.for_sync_relay = data;
      //std::cout << "set data :" << it->second.for_sync_relay->data_string;
      break;
    }
  }
  ez_timer_.Stop();
  return false;
}

boost::shared_ptr<IpcJsonMessage> MessageManager::GetData(int message_id, int listener_id) {
  boost::shared_ptr<IpcJsonMessage> buffer;
  std::map<std::pair<int,int>, ExtendedInfo >::iterator it;
  {
    boost::unique_lock<boost::mutex> lock(mtx_);
    if (sync_message_list.count(std::make_pair(message_id, listener_id)) > 0) {
      for (it = sync_message_list.begin(); it != sync_message_list.end(); ++it) {
        if (it->first.first == message_id && it->first.second == listener_id) {

          buffer = boost::move(it->second.for_sync_relay);
          //std::cout << "get data :" << it->second.for_sync_relay->data_string;
          break;
        }
      }
    }
  }
  Remove(message_id, listener_id);
  wait_sync_id_ = std::make_pair(-1, -1);

  return buffer;
}

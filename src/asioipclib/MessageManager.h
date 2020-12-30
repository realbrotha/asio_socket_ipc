//
// Created by realbro on 12/14/20.
//

#ifndef INTEGRATION_AGENT_SRC_LIB_COMMONEX_UNIXDOMAINSOCKET_MESSAGEMANAGER_H_
#define INTEGRATION_AGENT_SRC_LIB_COMMONEX_UNIXDOMAINSOCKET_MESSAGEMANAGER_H_

#include "EzTimer.h"
#include "MessageDefine.hpp"

#include "boost/thread/mutex.hpp"
#include <map>

struct ExtendedInfo {
  ExtendedInfo () :is_sync(false) {;}
  bool is_sync;
  int src_socket_fd;
  boost::shared_ptr<IpcJsonMessage> for_sync_relay;
};

class MessageManager {
 public :
  MessageManager();
  ~MessageManager();

  bool Initialize();
  bool Add(int message_id, int listener_id, int src);
  bool Add(int message_id, int listener_id, int src, boost::chrono::milliseconds msec);

  bool Remove(int message_id, int listener_id);
  int GetDest(int message_id, int listener_id);
  int IsSyncMessage(int message_id, int listener_id);
  bool IsRegisteredMessage(int message_id, int listener_id);
  bool SetData(int message_id, int listener_id, boost::shared_ptr<IpcJsonMessage>& data);
  boost::shared_ptr<IpcJsonMessage> GetData(int message_id, int listener_id);

 private :

  void TimerOutCallback();
  boost::mutex mtx_;

  std::map<std::pair<int, int> /* msg_id, listener_id */, ExtendedInfo > sync_message_list;
  boost::atomic<bool> read_data_;
  std::pair<int,int> wait_sync_id_;
  EzTimer ez_timer_;
};

#endif //INTEGRATION_AGENT_SRC_LIB_COMMONEX_UNIXDOMAINSOCKET_MESSAGEMANAGER_H_

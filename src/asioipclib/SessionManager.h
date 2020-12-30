//
// Created by realbro on 12/29/20.
//

#ifndef INTEGRATION_AGENT_SRC_LIB_COMMONEX_UNIXDOMAINSOCKET_SESSIONMANAGER_H_
#define INTEGRATION_AGENT_SRC_LIB_COMMONEX_UNIXDOMAINSOCKET_SESSIONMANAGER_H_

#include <vector>
#include <map>
#include "asio_wrapper/session.h"
#include "EzTimer.h"
class SessionManager {
 public :
  SessionManager();
  ~SessionManager();

  std::vector<network::session* > GetAllofSocketList();

  bool AddTemporarySession(network::session* s);
  bool AddCompleteSession(int product_code, network::session *s);

  bool Add(int product_code, network::session* s);
  bool Remove(network::session* s);
  bool Remove(int socket_fd);
  bool GetSession(int product_code, network::session* out_session);

 private :
  void TimeOutCallback();

  EzTimer ez_timer_;
  std::map<network::session *, time_t> temporary_session_list_;
  std::map<int, network::session*> client_session_list_;
};

#endif //INTEGRATION_AGENT_SRC_LIB_COMMONEX_UNIXDOMAINSOCKET_SESSIONMANAGER_H_

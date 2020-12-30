//
// Created by realbro on 12/29/20.
//

#include "SessionManager.h"
#include "glog/logging.h"

#define SESSION_TIME_OUT 5

SessionManager::SessionManager() {
  ez_timer_.Initialize(boost::bind(&SessionManager::TimeOutCallback, this), false, false);
}
SessionManager::~SessionManager() {

}

void SessionManager::TimeOutCallback() {
  std::cout << "Timer Callback";
  std::map<network::session *, time_t>::iterator it;

  time_t current_time = time(NULL);
  for (it = temporary_session_list_.begin(); it != temporary_session_list_.end();) {
    time_t rhs = it->second;
    if (difftime(current_time, rhs) > SESSION_TIME_OUT) {
      //it->first->async_close();

      temporary_session_list_.erase(it++);
    } else {
      ++it;
    }
  }
  if (temporary_session_list_.empty()) {
    std::cout << "Stop";
    ez_timer_.Stop();
  }
}
std::vector<network::session *> SessionManager::GetAllofSocketList() {
  return std::vector<network::session *>();
}

bool SessionManager::Add(int product_code, network::session *s) {
  std::cout << "Add  Session";
  bool result = false;

  if (!client_session_list_.count(product_code)) {
    client_session_list_[product_code] = s;
    result = true;
  }
  return result;
}

bool SessionManager::AddCompleteSession(int product_code, network::session *s) {
  std::cout << "Add complete Session";
  bool result = false;

  std::map<network::session *, time_t>::iterator it;
  for (it = temporary_session_list_.begin(); it != temporary_session_list_.end();) {
    if (it->first == s) {
      temporary_session_list_.erase(it++);
      break;
    }
  }

  if (!client_session_list_.count(product_code)) {
    client_session_list_[product_code] = s;
    result = true;
  }
  return result;
}
bool SessionManager::AddTemporarySession(network::session *s) {
  std::cout << "Add Temporary Session";
  bool result = false;

  if (!temporary_session_list_.count(s)) {
    time_t current_time = time(NULL);
    temporary_session_list_[s] = current_time;
    result = true;
  }
  if (!temporary_session_list_.empty()) {
    std::cout << "EzTimer Start";
    ez_timer_.SetInterval(boost::chrono::milliseconds(3000));
    ez_timer_.Start();
    std::cout << "EzTimer ENd";
  }
  return result;
}

bool SessionManager::Remove(network::session *s) {
  bool result = false;
  std::map<int, network::session *>::iterator it;

  for (it = client_session_list_.begin(); it != client_session_list_.end(); ++it) {
    if (it->second == s) {
      client_session_list_.erase(it);
      result = true;
      break;
    }
  }
  return result;
}

bool SessionManager::Remove(int product_code) {
  bool result = false;
  std::map<int, network::session *>::iterator it;

  for (it = client_session_list_.begin(); it != client_session_list_.end(); ++it) {
    if (it->first == product_code) {
      client_session_list_.erase(it);
      result = true;
      break;
    }
  }
  return result;
}

bool SessionManager::GetSession(int product_code, network::session *out_session) {
  bool result = false;
  if (client_session_list_.count(product_code)) {
    out_session = client_session_list_[product_code];
    result = true;
  }
  return result;
}

//
// Created by realbro on 20. 9. 14..
//

#include "ConnectionManager.h"

ConnectionManager& ConnectionManager::GetInstance() {
  static ConnectionManager instance;
  return instance;
}

bool ConnectionManager::Add(int product_code, int socket_fd) {

  bool result = false;

  if (!client_socket_list_.count(product_code)) {
    client_socket_list_[product_code] = socket_fd;
    result = true;
  }
  return result;
}
std::vector<int> ConnectionManager::GetAllofSocketList() {
  std::vector<int> socket_list;

  std::map<int, int>::iterator it;
  for ( it = client_socket_list_.begin(); it !=client_socket_list_.end(); it++) {
    socket_list.push_back(it->second);
  }
  return socket_list;
}
int ConnectionManager::GetSocket(int product_code) {
  int result = -1;

  if (client_socket_list_.count(product_code)) {
    result = client_socket_list_[product_code];
  }
  return result;
}
bool ConnectionManager::Remove(int socket_fd) {
  bool result = false;
  std::map<int, int>::iterator it;

  for (it = client_socket_list_.begin(); it != client_socket_list_.end(); ++it) {
    if (it->second == socket_fd) {
      client_socket_list_.erase(it);
      result = true;
      break;
    }
  }
  return result;
}

ConnectionManager::ConnectionManager() {
  ;
}

ConnectionManager::~ConnectionManager() {
  client_socket_list_.clear();
}
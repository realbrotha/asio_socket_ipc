//
// Created by realbro on 9/9/20.
//

#ifndef TESTEPOLLPIPE_SRC_LIB_MESSAGEDEFINE_H_
#define TESTEPOLLPIPE_SRC_LIB_MESSAGEDEFINE_H_

#include <stdint.h>

#include <string>
#include <vector>
#include <iterator>
#include <string.h>

enum MessageType {
  kUnknown = 0,
  kHELLO = 1,
  kNOTYET = 10001
};

struct IpcJsonMessage
{
  //int32_t msg_type =0; // pairing
  MessageType msg_type;
  int32_t msg_id =0; // msg Id
  int32_t src = 0 ; // 클라이언트 간 통신에 사용할 때 요청에 대한 응답을 돌려줄 DstType으로 사용할 수 있음.
  int32_t dst = 0;
  bool synchronous = false;   // 동기/비동기 메시지 구분. 기본 비동기.
  std::string json_string;

  IpcJsonMessage() : msg_type(MessageType::kUnknown), msg_id(0), src(0), dst(0), synchronous(false), json_string("") { ; };
  IpcJsonMessage(MessageType type, int32_t id, int32_t source,  int32_t destination, bool sync_or_async, std::string data) {
    msg_type = type;
    msg_id = id;
    source = source;
    destination = destination;
    json_string = data;
    synchronous = sync_or_async;
  };
  virtual ~IpcJsonMessage() {
    ;
  };
  bool Serialize(std::vector<char> &out) {
    printf("Serialize\n");
    std::copy(reinterpret_cast<char *>(&msg_type), reinterpret_cast<char *>(&msg_type)  , std::back_inserter(out));
    std::copy(reinterpret_cast<char *>(&msg_id), reinterpret_cast<char *>(&msg_id)  + sizeof(msg_id), std::back_inserter(out));
    std::copy(reinterpret_cast<char *>(&src), reinterpret_cast<char *>(&src) + sizeof(src), std::back_inserter(out));
    std::copy(reinterpret_cast<char *>(&dst), reinterpret_cast<char *>(&dst)  + sizeof(dst), std::back_inserter(out));
    std::copy(reinterpret_cast<char *>(&synchronous),reinterpret_cast<char *>(&synchronous) + sizeof(synchronous), std::back_inserter(out));
    std::copy(json_string.begin(),json_string.end(), std::back_inserter(out));

    return true;
  };

  bool Deserialize(std::vector<char> &out) {
    printf("Deserialize\n");
    int last_index = 0;
    std::copy(out.begin() + last_index, out.begin() +last_index + sizeof(msg_type), reinterpret_cast<char *>(&msg_type));
    last_index += sizeof(msg_type);
    std::copy(out.begin() + last_index, out.begin() +last_index + sizeof(msg_id), reinterpret_cast<char *>(&msg_id));
    last_index += sizeof(msg_id);
    std::copy(out.begin() + last_index, out.begin() +last_index + sizeof(src), reinterpret_cast<char *>(&src));
    last_index += sizeof(src);
    std::copy(out.begin() + last_index, out.begin() +last_index + sizeof(dst), reinterpret_cast<char *>(&dst));
    last_index += sizeof(dst);
    std::copy(out.begin() + last_index, out.begin() +last_index + sizeof(synchronous), reinterpret_cast<char *>(&synchronous));
    last_index += sizeof(synchronous);
    std::copy(out.begin() + last_index, out.end(),  std::back_inserter(json_string));

    return true;
  };
};

#endif //TESTEPOLLPIPE_SRC_LIB_MESSAGEDEFINE_H_

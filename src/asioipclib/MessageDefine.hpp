//
// Created by realbro on 9/9/20.
//

#ifndef TESTEPOLLPIPE_SRC_LIB_MESSAGEDEFINE_H_
#define TESTEPOLLPIPE_SRC_LIB_MESSAGEDEFINE_H_

#include "boost/core/scoped_enum.hpp"
#include <stdint.h>

#include <string>
#include <vector>
#include <iterator>
#include <string.h>
#include "boost/enable_shared_from_this.hpp"

enum MESSAGE_TYPE
{
  UNKNOWN = 0,
  HELLO = 1,
};

enum PRODUCT_TYPE
{
  ASM_SERVER = 0x00,
  INTEGRATION_AGENT =0x01,
  PMS = 0x02,
  MPI = 0x04,
  VMS = 0x08,
};
#include <stdio.h>
struct IpcJsonMessage : public boost::enable_shared_from_this<IpcJsonMessage>
{
  //int32_t msg_type =0; // pairing
  int16_t listener_id;
  int32_t sequence_id; // msg Id
  int16_t src; // 클라이언트 간 통신에 사용할 때 요청에 대한 응답을 돌려줄 DstType으로 사용할 수 있음.
  int16_t dst;
  int32_t data_size;
  bool synchronous;   // 동기/비동기 메시지 구분. 기본 비동기.
  //std::string data;
  std::string data_string;
  int GetHeaderSize() {
    return sizeof(listener_id) +  sizeof(sequence_id) + sizeof(src)+sizeof(dst)+sizeof(data_size)+sizeof(synchronous);
  }
  IpcJsonMessage() : listener_id(0), sequence_id(0), src(0), dst(0), data_size(0), synchronous(false) { ; };
  IpcJsonMessage(int16_t type, int32_t id, int16_t source,  int16_t destination, bool sync_or_async, std::string string) {
    listener_id = type;
    sequence_id = id;
    source = source;
    destination = destination;
    data_string = string;
    data_size = string.length();
    synchronous = sync_or_async;
  };
  virtual ~IpcJsonMessage() {
    ;
  };

  bool Serialize(std::vector<char> &out) {
    std::copy(reinterpret_cast<char *>(&listener_id), reinterpret_cast<char *>(&listener_id) + sizeof(listener_id) , std::back_inserter(out));
    std::copy(reinterpret_cast<char *>(&sequence_id), reinterpret_cast<char *>(&sequence_id)  + sizeof(sequence_id), std::back_inserter(out));
    std::copy(reinterpret_cast<char *>(&src), reinterpret_cast<char *>(&src) + sizeof(src), std::back_inserter(out));
    std::copy(reinterpret_cast<char *>(&dst), reinterpret_cast<char *>(&dst)  + sizeof(dst), std::back_inserter(out));
    std::copy(reinterpret_cast<char *>(&data_size),reinterpret_cast<char *>(&data_size) + sizeof(data_size), std::back_inserter(out));
    std::copy(reinterpret_cast<char *>(&synchronous),reinterpret_cast<char *>(&synchronous) + sizeof(synchronous), std::back_inserter(out));
    std::copy(data_string.begin(), data_string.end(), std::back_inserter(out));
    return true;
  };

  bool Deserialize(std::vector<char> &in) {
    if ( in.size() < GetHeaderSize()) return false;
    int last_index = 0;
    std::copy(in.begin() + last_index, in.begin() +last_index + sizeof(listener_id), reinterpret_cast<char *>(&listener_id));
    last_index += sizeof(listener_id);
    std::copy(in.begin() + last_index, in.begin() +last_index + sizeof(sequence_id), reinterpret_cast<char *>(&sequence_id));
    last_index += sizeof(sequence_id);
    std::copy(in.begin() + last_index, in.begin() +last_index + sizeof(src), reinterpret_cast<char *>(&src));
    last_index += sizeof(src);
    std::copy(in.begin() + last_index, in.begin() +last_index + sizeof(dst), reinterpret_cast<char *>(&dst));
    last_index += sizeof(dst);
    std::copy(in.begin() + last_index, in.begin() +last_index + sizeof(data_size), reinterpret_cast<char *>(&data_size));
    last_index += sizeof(data_size);
    std::copy(in.begin() + last_index, in.begin() +last_index + sizeof(synchronous), reinterpret_cast<char *>(&synchronous));
    last_index += sizeof(synchronous);
    std::copy(in.begin() + last_index, in.end(), std::back_inserter(data_string));

    return true;
  };

  bool DeserializeHeader(const uint8_t *buff, const int32_t len) {
    if (len != GetHeaderSize() )
      return false;

    int last_index = 0;
    std::copy(buff + last_index, buff +last_index + sizeof(listener_id), reinterpret_cast<char *>(&listener_id));
    last_index += sizeof(listener_id);
    std::copy(buff + last_index, buff +last_index + sizeof(sequence_id), reinterpret_cast<char *>(&sequence_id));
    last_index += sizeof(sequence_id);
    std::copy(buff + last_index, buff +last_index + sizeof(src), reinterpret_cast<char *>(&src));
    last_index += sizeof(src);
    std::copy(buff + last_index, buff +last_index + sizeof(dst), reinterpret_cast<char *>(&dst));
    last_index += sizeof(dst);
    std::copy(buff + last_index, buff +last_index + sizeof(data_size), reinterpret_cast<char *>(&data_size));
    last_index += sizeof(data_size);
    std::copy(buff + last_index, buff +last_index + sizeof(synchronous), reinterpret_cast<char *>(&synchronous));
    last_index += sizeof(synchronous);

    return true;
  };
};

#endif //TESTEPOLLPIPE_SRC_LIB_MESSAGEDEFINE_H_

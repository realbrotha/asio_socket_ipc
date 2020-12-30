//
// Created by realbro on 19. 6. 18.
//

#ifndef UTIL_NETWORK_BUFFER_H
#define UTIL_NETWORK_BUFFER_H

#include <iostream>
#include <vector>
#include <stdint.h>
#include <boost/enable_shared_from_this.hpp>
namespace network {

class buffer : public boost::enable_shared_from_this<buffer> {
 public:
  buffer();
  buffer(const uint8_t *buff, const size_t len);
  void set_reserve_size_unit(const size_t size);
  size_t get_reserve_size_unit() const;
  size_t size() const;
  uint8_t *get();
  uint8_t *get(const size_t len);
  void copy(const uint8_t *buff, const size_t len);
  void copy(const size_t offset, const uint8_t *buff, const size_t len);
  void append(const void *data, const size_t data_size);
  void reset();
 private:
  std::vector<uint8_t> buff_;
  size_t size_;
  size_t reserve_size_unit_;
};

}

#endif //UTIL_NETWORK_BUFFER_H

//
// Created by realbro on 19. 7. 10.
//

#ifndef UTIL_NETWORK_CALLBACK_TIMER_H
#define UTIL_NETWORK_CALLBACK_TIMER_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include "typedef.h"
namespace util {
class callback_timer : public boost::enable_shared_from_this<callback_timer> {
 public:
  typedef boost::function<void(boost::system::error_code)> callback_type;

  callback_timer(network::io_context &io);
  ~callback_timer();
  static boost::shared_ptr<callback_timer> create(network::io_context &io,
                                                  callback_type callback,
                                                  const uint32_t interval_msec,
                                                  const uint32_t delay_msec = 0,
                                                  uint32_t loop_count = 0);

  // loop_count
  // 0 : unlimited
  boost::system::error_code start(callback_type callback,
                                  const uint32_t interval_msec,
                                  const uint32_t delay_msec,
                                  uint32_t loop_count = 0);
  boost::system::error_code restart(const uint32_t interval_msec, const uint32_t delay_msec);
  boost::system::error_code Stop();

 private:
  void callback(boost::system::error_code ec);
  callback_type callback_;
  uint32_t interval_msec_;
  network::timer timer_;
  uint32_t loop_count_max_;
  uint32_t loop_count_;
};
}
#endif //UTIL_NETWORK_CALLBACK_TIMER_H

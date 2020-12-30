//
// Created by realbro on 19. 7. 10.
//

#include "callback_timer.h"
#include <boost/bind.hpp>
#include <glog/logging.h>
#include "lib/util/defines/glog_wrapper.h"
#include "error_code.h"
namespace util {
callback_timer::callback_timer(network::io_context &io)
    : timer_(io) {}

callback_timer::~callback_timer() {
  std::cout << "callback_timer destructor.";
}

boost::shared_ptr<callback_timer> callback_timer::create(network::io_context &io,
                                                         callback_type callback,
                                                         const uint32_t interval_msec,
                                                         const uint32_t delay_msec,
                                                         uint32_t loop_count) {
  boost::shared_ptr<callback_timer> obj(new callback_timer(io));
  obj->start(callback, interval_msec, delay_msec, loop_count);
  return obj;
}

// loop_count
// 0 : unlimited
boost::system::error_code callback_timer::start(callback_type callback,
                                                const uint32_t interval_msec,
                                                const uint32_t delay_msec,
                                                uint32_t loop_count) {
  loop_count_max_ = loop_count;
  loop_count_ = 0;

  boost::system::error_code ec;
  interval_msec_ = interval_msec;
  callback_ = callback;

  int interval = (delay_msec) ? delay_msec : 1; // do not use zero

  timer_.expires_from_now(boost::posix_time::milliseconds(interval), ec);
  if (ec) {
    ec_logging(ERROR, ec);
    return ec;
  }

  timer_.async_wait(boost::bind(&callback_timer::callback, shared_from_this(),
                                boost::asio::placeholders::error));
  return boost::system::error_code();
}
boost::system::error_code callback_timer::restart(const uint32_t interval_msec, const uint32_t delay_msec) {
  loop_count_ = 0;
  boost::system::error_code ec;

  if (interval_msec)
  {
    interval_msec_ = interval_msec;
  }
  int interval = (delay_msec) ? delay_msec : 1; // do not use zero
  timer_.cancel(ec);
  timer_.expires_from_now(boost::posix_time::milliseconds(interval), ec);
  if (ec) {
    ec_logging(ERROR, ec);
    return ec;
  }
  timer_.async_wait(boost::bind(&callback_timer::callback, shared_from_this(),
                                boost::asio::placeholders::error));
  return ec;
}
boost::system::error_code callback_timer::Stop() {
  boost::system::error_code ec;
  timer_.cancel(ec);
  if (ec) {
    ec_logging(ERROR, ec);
  }
  return ec;
}

void callback_timer::callback(boost::system::error_code ec) {
  if (ec == boost::asio::error::operation_aborted)
  {
    std::cout<<"callback_timer is stopped.(oeration_aborted)";
    return;
  } else if (ec)
  {
    std::cout<<"callback_timer ERROR: "<<ec.category().name()<<", "<<ec.message();
    return;
  }
  callback_(ec);
  if (!(loop_count_max_ == 0 || ++loop_count_ < loop_count_max_)) {
    std::cout << "job is completed. (" << loop_count_ << "/" << loop_count_max_ << ")";
    return;
  }
  timer_.expires_from_now(boost::posix_time::milliseconds(interval_msec_), ec);
  if (ec) {
    ec_logging(ERROR, ec);
    return;
  }
  timer_.async_wait(boost::bind(&callback_timer::callback, shared_from_this(),
                                boost::asio::placeholders::error));
}
}
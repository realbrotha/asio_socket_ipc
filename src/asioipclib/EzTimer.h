//
// Created by realbro on 12/14/20.
//

#ifndef INTEGRATION_AGENT_SRC_BIN_IPCTESTER_SERVER_EZTIMER_H_
#define INTEGRATION_AGENT_SRC_BIN_IPCTESTER_SERVER_EZTIMER_H_

#include "boost/chrono.hpp"
#include "boost/thread.hpp"
#include "boost/function.hpp"
#include "boost/atomic.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread.hpp"
#include "boost/thread/scoped_thread.hpp"

class EzTimer {
 public:
  EzTimer();
  EzTimer(const boost::chrono::milliseconds &interval,
          const boost::function<void()> &task,
          bool single_shot,
          bool sync);
  ~EzTimer();

  void Initialize(const boost::function<void()> &task,
                  bool singleShot = false,
                  bool sync = false,
                  const boost::chrono::milliseconds interval = boost::chrono::milliseconds(0xFFFFFFFF));
  void Start();
  void Stop();
  void SetSingleShot(bool enabled = true);
  bool IsRunning() const;
  void TimerHandler();
  void SetInterval(const boost::chrono::milliseconds interval);
  typedef boost::scoped_thread<boost::join_if_joinable, boost::thread> scope_thread;
  scope_thread timer_;

 private:
  bool GetStatus();
  bool StartThread();
  void StopThread();

  boost::chrono::milliseconds msec_interval_;
  boost::function<void()> task_;
  boost::atomic_bool sync_;
  boost::atomic_bool single_shot_;
  boost::atomic_bool running_;
  boost::condition_variable condition_;
  boost::mutex run_mutex_;
  boost::mutex stop_mutex_;

};

#endif //INTEGRATION_AGENT_SRC_BIN_IPCTESTER_SERVER_EZTIMER_H_

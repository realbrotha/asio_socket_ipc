//
// Created by realbro on 12/14/20.
//

#include "EzTimer.h"
#include "glog/logging.h"
EzTimer::EzTimer() : running_(false) {

}
EzTimer::EzTimer(const boost::chrono::milliseconds &interval,
                 const boost::function<void()> &task,
                 bool single_shot,
                 bool sync)
    : msec_interval_(interval),
      task_(task),
      sync_(sync),
      single_shot_(single_shot),
      running_(false) {
}

EzTimer::~EzTimer() {
  Stop();
}

void EzTimer::Initialize(const boost::function<void()> &task,
                         bool singleShot,
                         bool sync,
                         const boost::chrono::milliseconds interval) {
  msec_interval_ = interval;
  task_ = task;
  single_shot_ = singleShot;
  sync_ = sync;
}

void EzTimer::SetInterval(const boost::chrono::milliseconds interval) {
  msec_interval_ = interval;
}

bool EzTimer::IsRunning() const {
  return running_;
}

void EzTimer::Start() {
  Stop();

  running_ = true;
  StartThread();
}
bool EzTimer::StartThread() {
  bool result = false;

  if (timer_.joinable()) {
    std::cout << "thread already exist";
    return true;
  }
  try {
    scope_thread worker(BOOST_THREAD_MAKE_RV_REF(boost::thread(&EzTimer::TimerHandler, boost::ref(*this))));
    timer_ = BOOST_THREAD_MAKE_RV_REF(scope_thread(boost::move(worker)));
    if (sync_ && timer_.joinable()) timer_.join();
    result = true;
  } catch (...) {
    std::cout << "thread exception";
  }
  return result;
}
void EzTimer::StopThread() {
  if (timer_.joinable()) {
    if (boost::this_thread::get_id() == timer_.get_id()) {
      timer_.detach();
    } else {
      try {
        timer_.join();
      } catch (const std::exception &ex) {
        std::cout << "exceoption -1";
      } catch (...) {
        std::cout << "exceoption -2";
      }
    }
  }
}
bool EzTimer::GetStatus() {
  return !running_;
}
void EzTimer::TimerHandler() {
  while (running_) {
    boost::unique_lock<boost::mutex> lock(run_mutex_);
    bool result = condition_.wait_for(lock, msec_interval_, boost::bind(&EzTimer::GetStatus, this));
    if (running_ && !result)
      task_();

    if (single_shot_)
      running_ = false;
  }
}

void EzTimer::SetSingleShot(bool enabled) {
  single_shot_ = enabled;
}

void EzTimer::Stop() {
  boost::unique_lock<boost::mutex> lock(stop_mutex_);
  running_ = false;
  condition_.notify_all();
  StopThread();
}
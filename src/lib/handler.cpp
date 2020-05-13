#include "handler.h"

#include "app/face_detector.h"
#include "app/face_extractor.h"

using namespace suanzi;

template <typename T>
T MessageStack<T>::get() {
  std::unique_lock<std::mutex> lock(mutex_);
  while (stack_.empty()) cv_.wait_for(lock, std::chrono::milliseconds(10));
  T ret = stack_.back();
  stack_.pop_back();

  return ret;
}

template <typename T>
void MessageStack<T>::put(T &t) {
  std::unique_lock<std::mutex> lock(mutex_);
  stack_.push_back(t);
  if (stack_.size() > kMaxSize) stack_.erase(stack_.begin());
  cv_.notify_one();
}

template <typename T>
void Handler<T>::run() {
  while (_bRunning) {
    T message = stack_.get();
    handle(message);
  }
}

template <typename T>
void Handler<T>::post(T message) {
  stack_.put(message);
}

template class Handler<ExtractionMessage>;
template class Handler<DetectionMessage>;
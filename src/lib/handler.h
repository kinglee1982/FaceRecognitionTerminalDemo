#ifndef HANDLER_H
#define HANDLER_H

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "thread.h"

namespace suanzi {

struct _ExtractionMessage;
struct _DetectionMessage;

template <typename T>
class MessageStack {
 private:
  const int kMaxSize = 1;

 public:
  void put(T &t);
  T get();

 private:
  std::mutex mutex_;
  std::condition_variable cv_;
  std::vector<T> stack_;
};

template <typename T>
class Handler : public AbstractRunnable {
 public:
  virtual void post(T message);

 private:
  virtual void run();
  virtual void handle(T &message) = 0;

 private:
  MessageStack<T> stack_;
};

}  // namespace suanzi

#endif
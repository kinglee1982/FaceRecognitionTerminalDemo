#ifndef THREAD_H
#define THREAD_H

#include <memory>
#include <thread>

namespace suanzi {

class Runnable {
 public:
  typedef std::shared_ptr<Runnable> Ptr;
  virtual void run() = 0;
};

class AbstractRunnable : public Runnable {
 public:
  AbstractRunnable() {}
  static void stop() { _bRunning = false; }

 protected:
  static bool _bRunning;
};

class Thread : public Runnable {
 public:
  Thread(Runnable *pRunnable);
  void join();
  static void sleep(int ms);
  void setPriority(int priority);
  void setaffinity(int cpuId);

 private:
  virtual void run();

 private:
  Ptr _pRunnable;
  std::thread _thread;
  int _cupId;
};

}  // namespace suanzi

#endif

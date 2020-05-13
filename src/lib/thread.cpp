#include "thread.h"

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <vector>

using namespace suanzi;

bool AbstractRunnable::_bRunning = true;

Thread::Thread(Runnable *pRunnable) {
  _pRunnable = Ptr(pRunnable);
  _thread = std::thread(std::bind(&Thread::run, this));
}

void Thread::join() { _thread.join(); }

void Thread::sleep(int ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void Thread::run() { _pRunnable->run(); }

void Thread::setPriority(int priority) {
  int policy;
  sched_param sch;
  pthread_getschedparam(_thread.native_handle(), &policy, &sch);
  sch.sched_priority = priority;
  if (pthread_setschedparam(_thread.native_handle(), SCHED_FIFO, &sch)) {
    std::cout << "Failed to setschedparam: " << strerror(errno) << '\n';
  }
}

void Thread::setaffinity(int cpuId) {
  cpu_set_t set;
  CPU_ZERO(&set);
  CPU_SET(cpuId, &set);
  if (pthread_setaffinity_np(_thread.native_handle(), sizeof(set), &set) ==
      -1) {
    std::cout << "Failed to sched_setaffinity: " << strerror(errno) << '\n';
    exit(0);
  }
  _cupId = cpuId;
}
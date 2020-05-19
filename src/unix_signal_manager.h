#ifndef UNIXSIGNALMANAGER_H
#define UNIXSIGNALMANAGER_H

#include <QCoreApplication>

#include <signal.h>
#include <unistd.h>

class UnixSignalManager
{
 public:
  void static catchUnixSignals(std::initializer_list<int> quitSignals)
  {
    auto handler = [](int sig) -> void {
      // blocking and not aysnc-signal-safe func are valid
      printf("\nReceived signal (%d).\n", sig);
    };

    sigset_t blocking_mask;
    sigemptyset(&blocking_mask);
    for (auto sig : quitSignals)
      sigaddset(&blocking_mask, sig);

    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_mask = blocking_mask;
    sa.sa_flags = 0;

    for (auto sig : quitSignals)
      sigaction(sig, &sa, nullptr);
  }
};

#endif  // UNIXSIGNALMANAGER_H

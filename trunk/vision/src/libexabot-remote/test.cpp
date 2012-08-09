#include <iostream>
#include <signal.h>
#include "libexabot-remote.h"

bool end = false;

void interrupt_signal(int s) {
  exa_remote_set_motors(0, 0);
  end = true;
}

int main(void) {
  exa_remote_initialize("192.168.1.2");
  signal(SIGINT, &interrupt_signal);

  while (!end) {
    exa_remote_set_motors(-0.5, 0.5);
    sleep(1);
  }

  exa_remote_deinitialize();
  return 0;
}

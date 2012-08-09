#ifndef __LIBEXABOT_REMOTE__
#define __LIBEXABOT_REMOTE__


extern "C" {
  bool exa_remote_initialize(const char* hostname); // host o ip del robot
  void exa_remote_deinitialize(void);
  
  void exa_remote_set_motors(float left, float right); // left,right en [-1,1]
}

#endif

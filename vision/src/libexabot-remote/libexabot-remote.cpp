#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "libexabot-remote.h"
using namespace std;
using boost::asio::ip::udp;

udp::socket* s;
udp::endpoint receiver_endpoint;
boost::asio::io_service io_service;

extern "C"
bool exa_remote_initialize(const char* hostname)
{
  udp::resolver resolver(io_service);
  udp::resolver::query query(udp::v4(), hostname, "7654");  
  receiver_endpoint = *resolver.resolve(query);
  
  s = new udp::socket(io_service);
  s->open(udp::v4());
  return true;
}

void exa_remote_deinitialize(void) {
  delete s;
  s = NULL;
}

void exa_remote_set_motors(float left, float right) {
  if (left > 1) left = 1; else if (left < -1) left = -1;
  if (right > 1) right = 1; else if (right < -1) right = -1;
  
  char cmd[3];
  cmd[0] = 0x3;
  cmd[1] = (char)roundf(right * 30);
  cmd[2] = (char)roundf(left * 30);
  s->send_to(boost::asio::buffer(cmd, 3), receiver_endpoint);
}

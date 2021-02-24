#ifndef __CLIENTINFO_H__
#define __CLIENTINFO_H__
#include <fmt/core.h>
#include <string>

struct clientInfo {
  int fd{-1};
  std::string name;
  std::string ip;
  uint16_t port;
  std::string toString() {
    return fmt::format("{:06}{:20}{:10}{:05}", fd, name, ip, port);
  }
  void fromString(std::string& data) {
    (void)data;
    fd = std::stoi(data.substr(0, 6));
    name = data.substr(6, 20);
    ip = data.substr(26, 10);
    port = std::stoi(data.substr(36));
  }
};

#endif  // __CLIENTINFO_H__
#ifndef __UNIXSOCK_H__
#define __UNIXSOCK_H__
#include <string>

class unixsock {
 public:
  unixsock(std::string server_path, std::string client_path);
  ~unixsock();
  bool readClient(int& fd, std::string& data);
  bool sendClient(int& fd, std::string& data);
  bool sendData(int client_fd, int fd, std::string& data);
  bool recvMsg(int client_fd, int& fd, std::string& data);

 private:
  std::string server_path_;
  std::string client_path_;
};

#endif  // __UNIXSOCK_H__
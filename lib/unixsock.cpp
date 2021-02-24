#include "unixsock.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string>

bool unixsock::sendClient(int& fd, std::string& data) {
  int server_sock = 0;
  int client_sock = 0;
  socklen_t len = 0;
  struct sockaddr_un server_sockaddr;
  struct sockaddr_un client_sockaddr;
  unlink(server_path_.c_str());
  if ((server_sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    return false;
  server_sockaddr.sun_family = AF_UNIX;
  strcpy(server_sockaddr.sun_path, server_path_.c_str());
  if (bind(server_sock, (struct sockaddr*)&server_sockaddr,
           sizeof(server_sockaddr)) < 0) {
    close(server_sock);
    return false;
  }

  if (listen(server_sock, 10) != 0) {
    return false;
  }
  if ((client_sock =
           accept(server_sock, (struct sockaddr*)&client_sockaddr, &len)) < 0) {
    close(server_sock);
    close(client_sock);
    unlink(server_path_.c_str());
    return -1;
  }

  struct msghdr msg;

  sendData(client_sock, fd, data);

  recvmsg(client_sock, &msg, 0);
  close(client_sock);
  return true;
}

bool unixsock::sendData(int client_fd, int fd, std::string& data) {
  char tmp_buf[80] = {0};
  struct msghdr msg;
  struct cmsghdr* cmsg;
  struct iovec vector;

  strncpy(tmp_buf, data.data(), data.length());
  vector.iov_base = tmp_buf;
  vector.iov_len = sizeof(tmp_buf);

  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  msg.msg_iov = &vector;
  msg.msg_iovlen = 1;

  cmsg = (struct cmsghdr*)alloca(sizeof(struct cmsghdr) + sizeof(fd));
  cmsg->cmsg_len = sizeof(struct cmsghdr) + sizeof(fd);
  cmsg->cmsg_level = SOL_SOCKET;
  cmsg->cmsg_type = SCM_RIGHTS;
  memcpy(CMSG_DATA(cmsg), &fd, sizeof(fd));
  msg.msg_control = cmsg;
  msg.msg_controllen = cmsg->cmsg_len;
  if (sendmsg(client_fd, &msg, 0) < 0)
    return false;
  return true;
}

bool unixsock::recvMsg(int client_fd, int& fd, std::string& data) {
  char tmp_buf[80] = {0};
  struct iovec vector;
  struct msghdr msg;
  struct cmsghdr* cmsg;
  int sockfd = -1;

  vector.iov_base = tmp_buf;
  vector.iov_len = sizeof(tmp_buf);

  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  msg.msg_iov = &vector;
  msg.msg_iovlen = 1;

  cmsg = (struct cmsghdr*)alloca(sizeof(struct cmsghdr) + sizeof(sockfd));
  cmsg->cmsg_len = sizeof(struct cmsghdr) + sizeof(sockfd);
  msg.msg_control = cmsg;
  msg.msg_controllen = cmsg->cmsg_len;

  if (recvmsg(client_fd, &msg, 0) < 0)
    return false;

  memcpy(&fd, CMSG_DATA(cmsg), sizeof(fd));
  data = std::string(tmp_buf);
  return true;
}
unixsock::~unixsock() {}

bool unixsock::readClient(int& fd, std::string& data) {
  int client_sock = -1;
  struct sockaddr_un client_sockaddr;
  struct sockaddr_un server_sockaddr;

  if ((client_sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
    return false;
  }
  client_sockaddr.sun_family = AF_UNIX;
  strcpy(client_sockaddr.sun_path, client_path_.c_str());

  unlink(client_path_.c_str());
  if (bind(client_sock, (struct sockaddr*)&client_sockaddr,
           sizeof(client_sockaddr)) < 0) {
    close(client_sock);
    return false;
  }
  server_sockaddr.sun_family = AF_UNIX;
  strcpy(server_sockaddr.sun_path, server_path_.c_str());
  if (connect(client_sock, (struct sockaddr*)&server_sockaddr,
              sizeof(server_sockaddr)) < 0) {
    close(client_sock);
    return false;
  }

  recvMsg(client_sock, fd, data);
  close(client_sock);
  unlink(client_path_.c_str());
  return true;
}
unixsock::unixsock(std::string server_path, std::string client_path)
    : server_path_(server_path), client_path_(client_path) {}

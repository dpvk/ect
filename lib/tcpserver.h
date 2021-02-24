#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__
#include <spdlog/spdlog.h>
#include <asio.hpp>
#include <string>
#include "clientinfo.h"
using namespace asio::ip;
class tcpServer {
 public:
  tcpServer(asio::io_context& io, uint16_t port, std::string responsemsg);
  ~tcpServer();
  clientInfo& client() { return client_; };
  void setClient(clientInfo& client);
  void start();

 private:
  void accept_async();
  void onConnected_(std::error_code, asio::ip::tcp::socket& ep);
  void readAsyncNtwkData_();
  asio::streambuf response_;

  clientInfo client_;

  uint16_t port_;
  asio::io_service& io_;
  asio::ip::tcp::socket socket_;
  std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;
  std::string responsemsg_;
};

#endif  // __TCPSERVER_H__
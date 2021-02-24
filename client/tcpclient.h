#ifndef __TCPCLIENT_H__
#define __TCPCLIENT_H__
#include <spdlog/spdlog.h>
#include <asio.hpp>
#include <chrono>
#include "clientinfo.h"

using namespace asio::ip;
class tcpclient {
 public:
  tcpclient(asio::io_context& io, clientInfo& info);
  ~tcpclient();
  bool connect();
  bool disconnect();
  void reconnectLater();

 private:
  bool connecting_{false};
  asio::io_context& io_;
  tcp::socket socket_;
  clientInfo info_;
  std::string request_message_{"request_data_from_server\n"};
  bool readAsyncNtwkData_();
  void sendMessage_();
  asio::streambuf response_;
  bool onConnected_(std::error_code ec, asio::ip::tcp::endpoint ep);
  asio::steady_timer t_;
  asio::steady_timer send_timer_;
};

#endif
#include "tcpclient.h"
#include <functional>
#include "spdlog/fmt/bin_to_hex.h"

tcpclient::tcpclient(asio::io_context& io, clientInfo& info)
    : io_(io), socket_(io_), info_(info), t_(io_), send_timer_(io_) {}

tcpclient::~tcpclient() {}

bool tcpclient::connect() {
  spdlog::info("Trying resolve [{}]", info_.ip);
  asio::ip::tcp::resolver resolver(io_);
  auto endpoints = resolver.resolve(info_.ip, std::to_string(info_.port));
  asio::async_connect(socket_, endpoints,
                      [this](std::error_code ec, asio::ip::tcp::endpoint ep) {
                        if (!ec) {
                          onConnected_(ec, ep);
                        } else {
                          spdlog::error("Connection error [{}]", ec.message());
                          connecting_ = false;
                          socket_.close();
                          reconnectLater();
                        }
                      });
  spdlog::info("Start connecting");
  return true;
}

void tcpclient::reconnectLater() {
  if (connecting_)
    return;
  connecting_ = true;
  t_.expires_after(std::chrono::seconds(5));
  t_.async_wait([&](std::error_code ec) {
    (void)ec;
    connect();
  });
}

bool tcpclient::readAsyncNtwkData_() {
  asio::async_read_until(socket_, response_, "\n",
                         [this](std::error_code ec, std::size_t) {
                           std::string data;
                           std::istream(&response_) >> data;
                           response_.consume(response_.size() + 1);
                           spdlog::info("Received  [{}]", data);
                           if (!ec) {
                             sendMessage_();
                             readAsyncNtwkData_();
                           } else {
                             spdlog::error("recv error [{}]", ec.message());
                             socket_.close();
                             reconnectLater();
                           }
                         });
  return true;
}

void tcpclient::sendMessage_() {
  try {
    std::string request_message_{"request_data_from_server\n"};
    asio::write(socket_, asio::buffer(request_message_.data(),
                                      request_message_.length()));
  } catch (const std::exception& e) {
    spdlog::error("Write error [{}]", e.what());
    socket_.close();
    reconnectLater();
  }

  // asio::async_write(
  //     socket_, asio::buffer(request_message_.data(),
  //     request_message_.length()), [this](std::error_code ec, std::size_t
  //     length) {
  //   if (!ec) {
  //     // spdlog::info("Succesfully sent  [{}] bytes", length);
  //   } else {
  //     spdlog::error("write error [{}]", ec.message());
  //     socket_.close();
  //     reconnectLater();
  //   }
  //     });
}

bool tcpclient::onConnected_(std::error_code ec, asio::ip::tcp::endpoint ep) {
  (void)ec;
  (void)ep;
  connecting_ = false;
  spdlog::info("Connection established [{}{}]", ep.address().to_string(),
               ep.port());

  sendMessage_();
  readAsyncNtwkData_();
  return true;
}

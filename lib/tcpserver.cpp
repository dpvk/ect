#include "tcpserver.h"

tcpServer::tcpServer(asio::io_context& io,
                     uint16_t port,
                     std::string responsemsg)
    : io_(io), socket_(io_), responsemsg_(responsemsg), port_(port) {}
void tcpServer::accept_async() {
  if (!acceptor_)
    return;
  acceptor_->async_accept(
      [this](std::error_code ec, 
             asio::ip::tcp::socket socket) {
        if (!ec) {
          onConnected_(ec, socket);
        }
        accept_async();
      });
}

void tcpServer::onConnected_(std::error_code, asio::ip::tcp::socket& ep) {
  spdlog::info("Connection established [{}{}]",
               ep.remote_endpoint().address().to_string(),
               ep.remote_endpoint().port());
  socket_ = std::move(ep);
  client_.fd = socket_.native_handle();
  client_.name = "Some client.";
  client_.ip = socket_.remote_endpoint().address().to_string();
  client_.port = socket_.remote_endpoint().port();

  //    read_header();
  readAsyncNtwkData_();
};

void tcpServer::readAsyncNtwkData_() {
  asio::async_read_until(
      socket_, response_, "\n",
      [this](std::error_code ec, std::size_t /*length*/) {
        std::string data;
        std::istream(&response_) >> data;
        spdlog::info("Received  [{}]", data);

        if (!ec) {
          asio::async_write(
              socket_, asio::buffer(responsemsg_.data(), responsemsg_.length()),
              [this](std::error_code ec, std::size_t length) {
                if (!ec) {
                  spdlog::info("Succesfully sent  [{}] bytes", length);
                } else {
                  spdlog::error("write error [{}]", ec.message());
                  socket_.close();
                }
              });
          readAsyncNtwkData_();
        } else {
          spdlog::error("recv head error [{}]", ec.message());
          socket_.close();
        }
      });
}

void tcpServer::setClient(clientInfo& client) {
  client_ = client;
  socket_.assign(asio::ip::tcp::v4(), client_.fd);
  readAsyncNtwkData_();
}

void tcpServer::start() {
  acceptor_ = std::make_unique<asio::ip::tcp::acceptor>(
      io_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port_));
  accept_async();
}

tcpServer::~tcpServer() {}

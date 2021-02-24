#include <spdlog/spdlog.h>
#include <asio.hpp>
#include <string>
#include "tcpclient.h"

using namespace asio::ip;
int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  asio::io_context io;

  asio::signal_set signals(io, SIGINT, SIGTERM);
  signals.async_wait([&](const std::error_code& error, int signal_number) {
    (void)error;
    (void)signal_number;
    spdlog::info("Got signal. Stopping ctx.");
    io.stop();
  });
  spdlog::info("Starting client.");

  clientInfo client{-1, "Server",
                   "127.0.0.1", 7070};
  tcpclient t(io, client);
  t.connect();
  io.run();

  return 0;
}
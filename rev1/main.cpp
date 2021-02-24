#include <string.h>
#include "tcpserver.h"
#include "unixsock.h"
int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  asio::io_context io;
  asio::signal_set signals(io, SIGINT, SIGTERM, SIGUSR1);
  tcpServer s(io, 7070, "rev1\n");

  if (argc > 1 && strcmp(argv[1], "update") == 0) {
    spdlog::info("Wait for client information");
    unixsock pass("/tmp/server.sock", "/tmp/client.sock");
    int fd = -1;
    std::string data;
    if (!pass.readClient(fd, data)) {
      spdlog::error("Error receive connected client");
      return 1;
    }
    spdlog::info("Gor cliet [{}][{}]", fd, data);
  }
  s.start();

  signals.async_wait([&](const std::error_code& error, int signal_number) {
    (void)error;
    (void)signal_number;

    switch (signal_number) {
      case SIGUSR1: {
        spdlog::info("Got signal fot client pass.");
        clientInfo& client = s.client();
        std::string data = client.toString();
        unixsock pass("/tmp/server.sock", "/tmp/client.sock");
        pass.sendClient(client.fd, data);
      }
      default: {
        spdlog::info("Got interrupt signal. Stopping ctx.");
        io.stop();
      } break;
    }
  });

  spdlog::info("Starting server rev1.");

  io.run();

  return 0;
}
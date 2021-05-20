#include <folly/init/Init.h>
#include <folly/portability/GFlags.h>
#include <glog/logging.h>
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <thrift/lib/cpp2/transport/core/ThriftProcessor.h>
#include <thrift/lib/cpp2/transport/rocket/server/RocketRoutingHandler.h>

#include "EchoService.h"

DEFINE_int32(echo_port, 7778, "Echo Server port");

using apache::thrift::ThriftServer;
using apache::thrift::ThriftServerAsyncProcessorFactory;
using example::echo::EchoHandler;
using apache::thrift::RocketRoutingHandler;

std::unique_ptr<RocketRoutingHandler>
createRocketRoutingHandler(std::shared_ptr<ThriftServer> server) {
  return std::make_unique<RocketRoutingHandler>(*server);
}

template <typename ServiceHandler>
std::shared_ptr<ThriftServer> newServer(int32_t port) {
  auto handler = std::make_shared<ServiceHandler>();
  auto proc_factory =
      std::make_shared<ThriftServerAsyncProcessorFactory<ServiceHandler>>(
          handler);
  auto server = std::make_shared<ThriftServer>();
  server->setPort(port);
  server->setProcessorFactory(proc_factory);
  server->addRoutingHandler(createRocketRoutingHandler(server));
  return server;
}

int main(int argc, char **argv) {
  FLAGS_logtostderr = 1;
  folly::init(&argc, &argv);

  auto echo_server = newServer<EchoHandler>(FLAGS_echo_port);
  LOG(INFO) << "Echo Server running on port: " << FLAGS_echo_port;
  echo_server->serve();

  return 0;
}

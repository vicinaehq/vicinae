#include <concepts>
#include <glaze/core/common.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <qfuture.h>
#include <variant>

namespace codegen {

struct JsonRpcRequest {
  std::string jsonrpc;
  std::string method;
  int id;
  glz::raw_json params;
};

struct JsonRpcNotification {
  std::string jsonrpc;
  std::string method;
  glz::raw_json params;
};

struct JsonRpcErrorResponse {
  std::string jsonrpc;
  int id;
  std::string error;
};

struct JsonRpcResponse {
  std::string jsonrpc;
  int id;
  glz::raw_json result;
};

using IncomingJsonRpcMessage = std::variant<JsonRpcRequest, JsonRpcNotification>;
using OutgoingJsonRpcMessage = std::variant<JsonRpcResponse, JsonRpcErrorResponse>;

class AbstractTransport {
public:
  using MessageHandler = std::function<void(const IncomingJsonRpcMessage &)>;
  virtual void onMessage(const MessageHandler &message) {}
  virtual ~AbstractTransport() = default;
};

template <typename Ctx> class AbstractMathsService {
public:
  virtual QFuture<double> add(double x, double y) = 0;
};

template <typename T, typename U>
concept DerivedFrom = std::derived_from<T, U>;

template <typename UserContext, DerivedFrom<AbstractMathsService<UserContext>> Maths> class Server {
public:
  Server(AbstractTransport *transport, UserContext uctx) : m_maths(std::make_unique<Maths>()) {
    transport->onMessage([](const IncomingJsonRpcMessage &msg) {
      if (auto req = std::get_if<JsonRpcRequest>(&msg)) {}
    });
  }

  template <glz::reflectable T> void reply(int id, const T &data) {
    static thread_local std::string buf;
    auto result = glz::write_json(data, buf);

    JsonRpcResponse{.id = id, .result = result};
  }

private:
  void handleRequest(const JsonRpcRequest &req) { m_maths->add(1, 2); }

  std::unique_ptr<AbstractMathsService<UserContext>> m_maths;
};

}; // namespace codegen

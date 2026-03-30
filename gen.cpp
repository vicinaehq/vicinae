#include <concepts>
#include <format>
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
  virtual void send(std::string_view data) {}
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

class RpcTransport {
  template <typename T> using Result = std::expected<T, std::string>;
  template <typename T> using Promise = QPromise<Result<T>>;
  template <typename T> using Handler = std::function<void(Result<T> result)>;

public:
  RpcTransport(AbstractTransport &transport) : m_transport(transport) {}

  std::expected<void, std::string> dispatchMessage(std::string_view data) {
    OutgoingJsonRpcMessage msg;

    if (auto const error = glz::read_json(msg, data)) { return std::unexpected(glz::format_error(error)); }

    if (auto res = std::get_if<JsonRpcResponse>(&msg)) {
      if (auto it = m_requestMap.find(res->id); it != m_requestMap.end()) {
        it->second();
        m_requestMap.erase(it);
      }
    }

    if (auto res = std::get_if<JsonRpcErrorResponse>(&msg)) {
      if (auto it = m_requestMap.find(res->id); it != m_requestMap.end()) {
        it->second();
        m_requestMap.erase(it);
      }
    }

    return {};
  }

  template <typename T, typename U>
  QFuture<std::expected<T, std::string>> request(std::string_view method, const U &params) {
    if (auto const error = glz::write_json(params, m_buf)) {
      return QtFuture::makeReadyValueFuture(std::unexpected(glz::format_error(error)));
    }

    auto promise = std::make_shared<Promise<T>>();
    auto future = promise.future();
    auto handler = [promise](Result<T> data) {
      auto value = data.and_then([](std::string_view data) -> Result<T> {
        T payload;
        if (auto const error = glz::read_json(payload, data)) {
          return std::unexpected(glz::format_error(error));
        }
        return payload;
      });

      promise->addResult(std::move(value));
      promise->finish();
    };

    int id = m_id++;
    m_requestMap.insert(id, handler);
    sendMessage(JsonRpcRequest{.method = std::string{method}, .id = id, .params = m_buf});

    return future;
  }

  template <typename T> void subscribe(std::string_view method, std::function<void(const T &result)> cb) {
    auto handler = createHandler(cb);
    if (auto it = m_handlers.find(std::string{method}); it != m_handlers.end()) {
      it->second.emplace_back(handler);
    } else {
      it->second = {handler};
    }
  }

  void sendMessage(const IncomingJsonRpcMessage &msg) {
    [[maybe_unused]] auto const res = glz::write_json(msg, m_buf);
    m_transport.send(m_buf);
  }

  int m_id = 0;
  std::unordered_map<std::string, std::vector<Handler>> m_handlers;
  std::unordered_map<int, std::function<void()>> m_requestMap;
  AbstractTransport &m_transport;
  std::string m_buf;
};
}; // namespace codegen

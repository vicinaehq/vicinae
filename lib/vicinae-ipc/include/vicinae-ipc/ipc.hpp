#pragma once
#include <concepts>
#include <cstdint>
#include <expected>
#include <format>
#include <glaze/core/meta.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/core/write.hpp>
#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/read.hpp>
#include <glaze/util/string_literal.hpp>
#include <numbers>
#include <string>
#include <variant>

namespace ipc {

enum class ClientType : std::uint8_t { CommandDispatcher, BrowserExtension };

template <typename T>
concept IsCommandVerb = requires {
  { T::key } -> std::convertible_to<std::string>;
  typename T::Request;
  typename T::Response;
};

struct Handshake {
  static constexpr const auto key = "handshake";

  struct Request {
    ClientType clientType;
  };

  struct Response {
    std::string version;
    std::int64_t pid;
  };
};

struct Deeplink {
  static constexpr const auto key = "deeplink";

  struct Request {
    std::string url;
  };

  struct Response {
    std::optional<std::string> error;
  };
};

struct LaunchApp {
  static constexpr const auto key = "launch-app";

  struct Request {
    std::string appId;
    std::vector<std::string> args;
    bool newInstance = false;
  };

  struct Response {
    std::optional<std::string> error;
    std::string focusedWindowTitle;
  };
};

struct AppInfo {
  std::string id;
  std::string name;
  std::string hidden;
  std::string path;
  std::string description;
  std::string program;
  bool isTerminalApp;
  std::vector<std::string> keywords;
  bool isAction;
};

struct ListApps {
  static constexpr const auto key = "list-apps";
  struct Request {
    bool withActions = false;
  };

  struct Response {
    std::vector<AppInfo> apps;
  };
};

struct Ping {
  static constexpr const auto key = "ping";
  struct Request {};
  struct Response {
    bool ok;
  };
};

struct DMenu {
  static constexpr const auto key = "dmenu";
  struct Request {
    std::string rawContent;
    std::optional<std::string> navigationTitle;
    std::optional<std::string> placeholder;
    std::optional<std::string> sectionTitle;
    bool noSection = false;
    bool noQuickLook = false;
    bool noIcon = false;
    bool noMetadata = false;
    std::optional<std::string> query;
    std::optional<int> width;
    std::optional<int> height;
    bool noFooter = false;
  };

  struct Response {
    std::string output; // empty if error or dismiss
  };
};

struct BrowserTabInfo {
  int id;
  int windowId;
  std::string title;
  std::string url;
  bool active;
};

struct BrowserTabsChanged {
  static constexpr const auto key = "browser-tabs-changed";

  struct Request {
    std::vector<BrowserTabInfo> tabs;
  };

  struct Response {};
};

/*
template <typename... Ts> struct variant_types<std::variant<Ts...>> {
  static constexpr auto ids = std::array{Ts::key...};
};
*/

template <typename... Ts> struct TypeList {};

using MyTypes = TypeList<int, double, std::string>;

template <typename List> struct Instantiator;

template <glz::rpc::concepts::method_type... Ts> struct Rpc {
  using Server = glz::rpc::server<Ts...>;
  using Client = glz::rpc::client<Ts...>;
};

template <IsCommandVerb... Types> struct RequestType {
  using type = std::variant<typename Types::Request...>;
  static constexpr const auto keys = std::array{Types::key...};
};

template <IsCommandVerb... Types> struct ResponseType {
  using type = std::variant<typename Types::Response...>;
  static constexpr const auto keys = std::array{Types::key...};
};

using CommandTypes = TypeList<Handshake>;

using RequestTypeVariant =
    RequestType<Ping, Handshake, Deeplink, LaunchApp, ListApps, DMenu, BrowserTabsChanged>::type;

static constexpr const auto RequestKeys =
    RequestType<Ping, Handshake, Deeplink, LaunchApp, ListApps, DMenu, BrowserTabsChanged>::keys;

using ResponseTypeVariant =
    ResponseType<Ping, Handshake, Deeplink, LaunchApp, ListApps, DMenu, BrowserTabsChanged>::type;

struct ResponseError {
  int code;
  std::string error;
};

template <IsCommandVerb... Ts> constexpr auto makeKeyArray() { return std::array{Ts::key...}; }

template <typename Variant> struct variant_types;

struct ErrorContext {
  int code;
  std::string message;
};

template <IsCommandVerb... Ts> struct RpcSchema {
  using RequestVariant = RequestType<Ts...>::type;
  using ResponseVariant = ResponseType<Ts...>::type;

  template <typename T> static constexpr bool contains = (std::same_as<T, Ts> || ...);

  struct Response {
    int id;
    std::optional<ResponseVariant> result;
    std::optional<ErrorContext> error;
  };

  struct Request {
    int id;
    std::string method;
    RequestVariant data;

    Response makeResponse(ResponseVariant data) const { return Response(id, data); }

    Response makeErrorResponse(ErrorContext error) const { return Response(id, {}, error); }
  };

  static std::expected<Request, std::string> parseRequest(std::string_view buf) {
    Request req;
    if (const auto error = glz::read_json(req, buf)) { return std::unexpected(glz::format_error(error)); }
    return req;
  }
};

template <typename T>
concept IsRpcSchema = requires {
  typename T::RequestVariant;
  typename T::ResponseVariant;
  typename T::Request;
  typename T::Response;
};

template <typename T, typename Schema>
concept InSchema = Schema::template contains<T>;

template <typename Schema> struct RpcClient {
  template <InSchema<Schema> T> typename std::string request(T::Request payload) {
    typename Schema::Request req{.id = 0, .method = T::key, .data = payload};
    std::string buf;

    if (const auto error = glz::write_json(req, buf)) {}

    return buf;
  }

private:
};

/*
template <typename Schema> class RpcServer {
public:
  struct ContextHandle {
    Context::GlobalContext *global = nullptr;
    Context::CallerContext *caller = nullptr;
  };

  RpcServer(Context::GlobalContext ctx) : m_ctx(ctx) {}

  using HRet = std::expected<PromiseLike<ipc::ResponseTypeVariant>, std::string>;
  using Handler = std::function<HRet(const ipc::RequestTypeVariant &req, ContextHandle ctx)>;
  using MiddlewareHandler =
      std::function<std::optional<std::string>(const ipc::RequestTypeVariant &request, ContextHandle ctx)>;

  template <ipc::IsCommandVerb T>
  void route(std::function<std::expected<PromiseLike<typename T::Response>, std::string>(
                 const typename T::Request &reqData, ContextHandle ctx)>
                 fn) {
    std::string method = T::key;

    m_handlers[method] = [fn](const ipc::RequestTypeVariant &req, ContextHandle ctx) -> HRet {
      if (auto actualReq = std::get_if<typename T::Request>(&req)) {
        std::expected<PromiseLike<typename T::Response>, std::string> res = fn(*actualReq, ctx);

        if (!res) { return std::unexpected(res.error()); }

        if (auto future = std::get_if<QFuture<typename T::Response>>(&res.value())) {
          return HRet(
              future->then([](const T::Response &response) { return ipc::ResponseTypeVariant(response); }));
        } else {
          typename T::Response actualRes = std::get<typename T::Response>(res.value());
          ipc::ResponseTypeVariant variant(actualRes);
          return HRet(variant);
        }
      } else {
        return std::unexpected("Mismatched request type");
      }
    };
  }

  void middleware(MiddlewareHandler fn) { m_middlewares.emplace_back(fn); }

  HRet call(const ipc::GenericRequest &req, typename Context::CallerContext &callerCtx) {
    ContextHandle handle(&m_ctx, &callerCtx);

    if (auto it = m_handlers.find(req.method); it != m_handlers.end()) {
      try {
        for (const auto &handler : m_middlewares) {
          if (const auto error = handler(req.data, handle)) { return std::unexpected(error.value()); }
        }
      } catch (const std::exception &e) { return std::unexpected(e.what()); }

      return it->second(req.data, handle);
    }

    return std::unexpected("No handler for method");
  }

private:
  Context::GlobalContext m_ctx;
  std::unordered_map<std::string, Handler> m_handlers;
  std::vector<MiddlewareHandler> m_middlewares;
};
*/

static void test() {
  using Schema = RpcSchema<Deeplink>;
  RpcClient<Schema> link;

  link.request<Deeplink>({});
}

}; // namespace ipc

template <> struct glz::meta<ipc::RequestTypeVariant> {
  static constexpr std::string_view tag = "method";
  static constexpr const auto ids = ipc::RequestKeys;
};

template <> struct glz::meta<ipc::ResponseTypeVariant> {
  static constexpr std::string_view tag = "method";
  static constexpr const auto ids = ipc::RequestKeys;
};

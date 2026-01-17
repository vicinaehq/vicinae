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
#include <glaze/json/write.hpp>
#include <glaze/util/string_literal.hpp>
#include <string>
#include <variant>

namespace ipc {

enum class ClientType : std::uint8_t { CommandDispatcher, BrowserExtension };

template <typename T>
concept IsCommandVerb = requires {
  { T::key } -> std::convertible_to<std::string>;
  typename T::Request;
  // typename T::Response;
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

struct BrowserInit {
  static constexpr const auto key = "browser/init";

  struct Request {
    std::string id;
    std::string name;
  };

  struct Response {};
};

struct BrowserTabInfo {
  int id;
  int windowId;
  std::string title;
  std::string url;
  bool active;
};

struct BrowserTabsChanged {
  static constexpr const auto key = "browser/tabs-changed";

  using Request = std::vector<BrowserTabInfo>;

  struct Response {};
};

struct FocusTab {
  static constexpr const auto key = "browser/focus-tab";
  struct Request {
    int tabId;
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

template <typename T>
concept hasResponse = requires { typename T::Response; };

template <IsCommandVerb... Ts> struct RpcSchema {
  using RequestVariant = RequestType<Ts...>::type;
  using ResponseVariant = ResponseType<Ts...>::type;

  template <typename T> static constexpr bool contains = (std::same_as<T, Ts> || ...);

  template <typename T> static constexpr bool isNotification = (std::same_as<T, Ts> || ...);

  struct Response {
    int id;
    std::optional<ResponseVariant> result;
    std::optional<ErrorContext> error;
  };

  struct Request {
    std::optional<int> id; // if no id, we assume we are dealing with a notification
    std::string method;
    RequestVariant data;

    Response makeResponse(ResponseVariant data) const { return Response(id.value_or(0), data); }

    Response makeErrorResponse(ErrorContext error) const { return Response(id.value_or(0), {}, error); }
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

template <typename SchemaT> struct RpcClient {
  using Schema = SchemaT;
  using RequestHandler = std::function<void(const typename Schema::ResponseVariant &var)>;

  template <InSchema<SchemaT> T> typename std::string request(T::Request payload) {
    typename SchemaT::Request req{.id = 0, .method = T::key, .data = payload};
    std::string buf;

    if (const auto error = glz::write_json(req, buf)) {}

    return buf;
  }

  /**
   * Send a notification, that is a request that does not expect a response
   */
  template <InSchema<SchemaT> T> typename std::string notify(T::Request payload) const {
    typename SchemaT::Request req{.method = T::key, .data = payload};
    std::string buf;
    if (const auto error = glz::write_json(req, buf)) {}
    return buf;
  }

  template <InSchema<SchemaT> T>
  typename std::string request(T::Request payload, std::function<void(const typename T::Response &res)> fn) {
    typename SchemaT::Request req{.id = 0, .method = T::key, .data = payload};
    std::string buf;

    if (const auto error = glz::write_json(req, buf)) {}

    m_requestMap[req.id] = [fn](const typename Schema::ResponseVariant &res) {
      if (auto typedRes = std::get_if<typename T::Response>(&res)) { fn(*typedRes); }
    };

    return buf;
  }

  std::expected<void, std::string> call(const typename Schema::Response &res) {
    if (auto it = m_requestMap.find(res.id); it != m_requestMap.end()) {
      it->second(res.result.value());
      m_requestMap.erase(it);
    }

    return std::unexpected(std::format("No request with this id {}", res.id));
  }

private:
  std::unordered_map<int, RequestHandler> m_requestMap;
};

template <IsRpcSchema Schema> class RpcServer {
public:
  using SchemaType = Schema;
  using HRet = std::expected<typename Schema::ResponseVariant, std::string>;
  using Handler = std::function<HRet(const typename Schema::RequestVariant &req)>;
  using NotificationHandler =
      std::function<std::expected<void, std::string>(const typename Schema::RequestVariant &req)>;
  using MiddlewareHandler =
      std::function<std::optional<std::string>(const typename Schema::RequestVariant &request)>;

  template <ipc::IsCommandVerb T>
    requires hasResponse<T>
  void
  route(std::function<std::expected<typename T::Response, std::string>(const typename T::Request &reqData)>
            fn) {
    std::string method = T::key;

    m_handlers[method] = [fn](const typename Schema::RequestVariant &req) -> HRet {
      if (auto actualReq = std::get_if<typename T::Request>(&req)) {
        return fn(*actualReq).transform([](auto &&value) { return typename Schema::ResponseVariant(value); });
      } else {
        return std::unexpected("Mismatched request type");
      }
    };
  }

  // notification overload - no response
  template <ipc::IsCommandVerb T>
    requires(!hasResponse<T>)
  void route(std::function<std::expected<void, std::string>(const typename T::Request &reqData)> fn) {
    std::string method = T::key;

    m_handlers[method] =
        [fn](const typename Schema::RequestVariant &req) -> std::expected<void, std::string> {
      if (auto actualReq = std::get_if<typename T::Request>(&req)) {
        return fn(*actualReq);
      } else {
        return std::unexpected("Mismatched request type");
      }
    };
  }

  void middleware(MiddlewareHandler fn) { m_middlewares.emplace_back(fn); }

  std::expected<std::string, std::string> call(const typename Schema::Request &req) {
    if (auto it = m_handlers.find(req.method); it != m_handlers.end()) {
      try {
        for (const auto &handler : m_middlewares) {
          if (const auto error = handler(req.data)) { return std::unexpected(error.value()); }
        }
      } catch (const std::exception &e) { return std::unexpected(e.what()); }

      auto resData = it->second(req.data);

      if (req.id) {
        typename Schema::Response res(req.id.value());

        if (!resData) {
          res.error = ErrorContext(-1, resData.error());
        } else {
          res.result = resData.value();
        }

        std::string buf;
        glz::write_json(res, buf);
        return buf;
      }

      return "";
    }

    return std::unexpected("No handler for method");
  }

private:
  std::unordered_map<std::string, Handler> m_handlers;
  std::unordered_map<std::string, NotificationHandler> m_notificationHandlers;
  std::vector<MiddlewareHandler> m_middlewares;
};

}; // namespace ipc

/*
template <typename T> struct glz::meta<ipc::RpcSchema<T>> {
  static constexpr std::string_view tag = "method";
  static constexpr const auto ids = ipc::RequestKeys;
};

template <> struct glz::meta<ipc::ResponseTypeVariant> {
  static constexpr std::string_view tag = "method";
  static constexpr const auto ids = ipc::RequestKeys;
};
*/

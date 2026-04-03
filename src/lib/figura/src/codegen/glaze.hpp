#pragma once
#include "codegen.hpp"
#include <format>
#include <iomanip>
#include <sstream>
#include "../utils.hpp"
#include <ranges>

constexpr const auto GLAZE_COMMON = R"(
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
  std::string method;
  int id;
  std::string error;
};

struct JsonRpcResponse {
  int id;
  std::string jsonrpc;
  glz::raw_json result;
};

using RpcMessage = std::variant<JsonRpcResponse, JsonRpcErrorResponse, JsonRpcNotification, JsonRpcRequest>;
using IncomingJsonRpcMessage = std::variant<JsonRpcRequest, JsonRpcNotification>;
using OutgoingJsonRpcMessage = std::variant<JsonRpcResponse, JsonRpcNotification, JsonRpcErrorResponse>;

class AbstractTransport {
public:
  virtual void send(std::string_view data) = 0;
  virtual ~AbstractTransport() = default;
};
)";

constexpr const auto GLAZE_CLIENT_CODE = R"(
struct RpcIncomingMessage {
  std::optional<int> id;
  std::optional<std::string> method;
  std::optional<std::string> error;
  glz::raw_json result;
  glz::raw_json params;
};

class RpcTransport {
  template <typename T> using Result = std::expected<T, std::string>;
  using ResponseHandler = std::function<void(Result<std::string_view>)>;

public:
  RpcTransport(AbstractTransport &transport) : m_transport(transport) {}

  std::expected<void, std::string> dispatchMessage(std::string_view data) {
    RpcIncomingMessage msg;

    if (auto const error = glz::read<glz::opts{.error_on_unknown_keys = false}>(msg, data)) { return std::unexpected(glz::format_error(error)); }

    if (msg.id) {
      if (auto it = m_requestMap.find(*msg.id); it != m_requestMap.end()) {
        if (msg.error) {
          it->second(std::unexpected(*msg.error));
        } else {
          it->second(std::move(msg.result.str));
        }
        m_requestMap.erase(it);
      }
    } else if (msg.method) {
      if (auto it = m_handlers.find(*msg.method); it != m_handlers.end()) {
        for (const auto &handler : it->second) {
          handler(msg.params.str);
        }
      }
    }

    return {};
  }

  template <typename T, typename U>
  void request(std::string_view method, const U &params, std::function<void(Result<T>)> cb) {
    if (auto const error = glz::write_json(params, m_buf)) {
      cb(std::unexpected(glz::format_error(error)));
      return;
    }

    int id = m_id++;
    auto sendRes = sendMessage(JsonRpcRequest{.jsonrpc = "2.0", .method = std::string{method}, .id = id, .params = m_buf});

    if (!sendRes) {
      cb(std::unexpected(std::move(sendRes).error()));
      return;
    }

    m_requestMap.insert({id, [cb = std::move(cb)](Result<std::string_view> data) {
      auto value = data.and_then([](std::string_view data) -> Result<T> {
        T payload;
        if (auto const error = glz::read_json(payload, data)) {
          return std::unexpected(glz::format_error(error));
        }
        return payload;
      });

      cb(std::move(value));
    }});
  }

  template <typename T>
  void subscribe(std::string_view method, std::function<void(const Result<T> &result)> cb) {
    auto handler = [cb = std::move(cb)](Result<std::string_view> data) {
      auto value = data.and_then([](std::string_view data) -> Result<T> {
        T payload;
        if (auto const error = glz::read_json(payload, data)) {
          return std::unexpected(glz::format_error(error));
        }
        return payload;
      });

      cb(value);
    };

    if (auto it = m_handlers.find(std::string{method}); it != m_handlers.end()) {
      it->second.emplace_back(handler);
    } else {
      m_handlers[std::string{method}] = {handler};
    }
  }

  std::expected<void, std::string> sendMessage(const IncomingJsonRpcMessage &msg) {
    if (auto const res = glz::write_json(msg, m_buf)) { return std::unexpected(glz::format_error(res)); }

    m_transport.send(m_buf);
    return {};
  }

private:
  int m_id = 1;
  std::unordered_map<std::string, std::vector<ResponseHandler>> m_handlers;
  std::unordered_map<int, ResponseHandler> m_requestMap;
  AbstractTransport &m_transport;
  std::string m_buf;
};

)";

constexpr const auto GLAZE_SERVER_BASE = R"(

class RpcTransport {
	public:
		RpcTransport(AbstractTransport& transport): m_transport(transport) {}

		template <typename T>
		void notify(std::string_view method, const T& params) {
			std::string paramsBuf;
			{
			[[maybe_unused]] auto res = glz::write_json(params, paramsBuf);
			}

			send(JsonRpcNotification{
				.jsonrpc = "2.0",
				.method = std::string{method},
				.params = paramsBuf
			});
		}

		template <typename T>
		void reply(int id, const T& result) {
			std::string resultBuf;
			{
			[[maybe_unused]] auto res = glz::write_json(result, resultBuf);
			}

			send(JsonRpcResponse{
				.id = id,
				.jsonrpc = "2.0",
				.result = resultBuf
			});
		}

		void replyError(int id, const std::string& error) {
			send(JsonRpcErrorResponse{.jsonrpc = "2.0", .id = id, .error = error});
		}

	private:
		void send(const OutgoingJsonRpcMessage& msg) {
			std::string buf;
			[[maybe_unused]] auto res = glz::write_json(msg, buf);
			m_transport.send(buf);
		}

		AbstractTransport& m_transport;
};

class EventEmitter {
	public:
		EventEmitter(RpcTransport& transport): m_transport(transport) {
		}

		template <typename T>
		void emitEvent(std::string_view name, const T& params) {
			m_transport.notify(name, params);
		}

	private:
		RpcTransport& m_transport;
};
)";

class GlazeGenerator : public AbstractCodeGenerator {
  std::string name() const override { return "glaze"; }

  static std::string_view serializeTypenameImpl(const TypeValue &value) {
    auto const visitor =
        overloads{[](TypeStruct *s) { return s->name; }, [](const EnumValue &e) { return e.name; },
                  [](PrimitiveType type) -> std::string_view {
                    switch (type) {
                    case PrimitiveType::Void:
                      return "void";
                    case PrimitiveType::Boolean:
                      return "bool";
                    case PrimitiveType::Number:
                      return "int";
                    case PrimitiveType::String:
                      return "std::string";
                    case PrimitiveType::Any:
                      return "glz::generic";
                    default:
                      std::unreachable();
                    }
                  }};

    return std::visit(visitor, value.data);
  }

  static std::string serializeTypename(const TypeValue &value) {
    std::string s{serializeTypenameImpl(value)};

    if (value.isArray) { s = std::format("std::vector<{}>", s); }
    if (value.isOptional) { s = std::format("std::optional<{}>", s); }

    return s;
  }

  static std::string abstractName(std::string_view className) { return std::string{"Abstract"} + className; }

  static std::string constRef(const TypeValue &value) {
    return std::string{"const "} + serializeTypename(value) + "&";
  }

  static std::string serializeEnumGlazeMeta(std::string_view ns, const EnumValue &e) {
    std::ostringstream oss;

    oss << "template <>";
    oss << "struct glz::meta<" << ns << "::" << e.name << "> {\n";
    oss << "\tusing enum " << ns << "::" << e.name << ";\n";
    oss << "\tstatic constexpr auto value = glz::enumerate(";

    for (const auto &[idx, v] : std::views::enumerate(e.values)) {
      if (idx > 0) oss << ", ";
      oss << v;
    }

    oss << ");";
    oss << "\n};\n";

    return oss.str();
  }

  static std::string serializeEnumGlazeMetas(std::string_view ns, std::span<EnumValue const> ee) {
    std::ostringstream oss;
    for (const auto &e : ee) {
      oss << serializeEnumGlazeMeta(ns, e);
    }
    return oss.str();
  }

  static std::string serializeEnum(const EnumValue &e) {
    std::ostringstream oss;

    oss << "enum class " << e.name << " {\n";
    for (const auto &v : e.values) {
      oss << "\t" << v << ",\n";
    }

    oss << "\n};\n";

    return oss.str();
  }

  static std::string serializeType(const TypeStruct &s) {
    std::ostringstream oss;

    oss << "struct " << s.name << " {\n";

    for (const auto &field : s.fields) {
      oss << "\t";
      oss << serializeTypename(field.type);
      oss << " " << field.name << ";\n";
    }

    oss << "\n};\n";

    return oss.str();
  }

  static std::string getMethodParamName(std::string_view serviceName, std::string_view mname) {
    return std::format("{}_{}_Params", serviceName, mname);
  }

  static std::string serializeEventParams(std::string_view serviceName, const Event &e) {
    return serializeMethodParams(serviceName,
                                 {.name = e.name, .params = e.params, .returnType = {PrimitiveType::Void}});
  }

  static std::string serializeMethodParams(std::string_view serviceName, const Method &method) {
    std::ostringstream oss;

    oss << "struct " << getMethodParamName(serviceName, method.name) << " {\n";

    for (const auto &param : method.params) {
      oss << "\t" << serializeTypename(param.type) << " " << param.name << ";\n";
    }

    oss << "\n};\n";

    return oss.str();
  }

  static void generateTypes(std::ostringstream &oss, const Tree &ast) {
    for (const auto &s : ast.enums) {
      oss << serializeEnum(s) << "\n";
    }

    for (const auto &s : ast.structs) {
      oss << serializeType(*s) << "\n";
    }

    for (const auto &s : ast.services) {
      for (const auto &e : s->events) {
        oss << serializeEventParams(s->name, e);
      }

      for (const auto &m : s->methods) {
        oss << serializeMethodParams(s->name, m);
      }
    }
  }

  static std::string_view stripExtension(std::string_view path) {
    auto const pos = path.find_last_of('.');
    if (pos != std::string_view::npos) return path.substr(0, pos);
    return path;
  }

  static std::string serializeService(const Service &s) {
    std::ostringstream oss;

    oss << "class " << abstractName(s.name) << ": public EventEmitter {\n";
    oss << "\tpublic:\n";
    oss << "\t" << abstractName(s.name) << "(RpcTransport& transport): EventEmitter(transport) {}\n\n";

    for (const auto &method : s.methods) {
      oss << "\t" << "virtual std::expected<" << serializeTypename(method.returnType) << ", std::string> "
          << method.name << "(";
      for (const auto &[idx, param] : method.params | std::views::enumerate) {
        if (idx > 0) oss << ", ";
        oss << serializeTypename(param.type) << " " << param.name;
      }
      oss << ") = 0;\n";
    }

    for (const auto &event : s.events) {
      oss << "\t" << "void emit" << event.name << " (";
      for (const auto &[idx, param] : event.params | std::views::enumerate) {
        if (idx > 0) oss << ", ";
        oss << constRef(param.type) << " " << param.name;
      }
      oss << ") {\n";

      std::string methodId = std::string{s.name} + "/" + event.name;

      auto eventParamsName = getMethodParamName(s.name, event.name);

      oss << "\t\temitEvent(" << std::quoted(methodId) << ", " << eventParamsName << "{";
      for (const auto &[idx, param] : event.params | std::views::enumerate) {
        if (idx > 0) oss << ", ";
        oss << "." << param.name << " = " << param.name;
      }
      oss << "});";

      oss << "\n}\n";
    }

    oss << "\n};\n";

    return oss.str();
  }

public:
  std::string generateClient(const Tree &ast, const CodegenOptions &opts) override {
    std::ostringstream oss;

    oss << R"(
#pragma once
#include <expected>
#include <functional>
#include <glaze/glaze.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>
	)";

    auto const ns =
        opts.generationNamespace.value_or(std::string{stripExtension(opts.file.filename().string())});

    oss << "namespace " << ns << " {\n";

    oss << GLAZE_COMMON << GLAZE_CLIENT_CODE;

    generateTypes(oss, ast);

    for (const auto &s : ast.services) {
      oss << "class " << s->name << "Service {\n";
      oss << "\tpublic:\n";
      oss << "\t" << s->name << "Service(RpcTransport& transport): m_transport(transport) {\n";

      for (const auto &m : s->events) {
        std::string methodId = std::format("{}/{}", s->name, m.name);
        oss << "transport.subscribe<" << getMethodParamName(s->name, m.name) << ">(" << std::quoted(methodId)
            << ", " << "[this](const auto& payload){ if (!payload) return;";
        oss << " if (m_" << m.name << "Handler) m_" << m.name << "Handler(";

        for (const auto &[idx, param] : m.params | std::views::enumerate) {
          if (idx > 0) { oss << ", "; }
          oss << "payload->" << param.name;
        }

        oss << ");";
        oss << "});\n";
      }

      oss << "}\n";

      for (const auto &m : s->methods) {
        std::string methodId = std::format("{}/{}", s->name, m.name);
        oss << "void " << m.name << "(";
        for (const auto &[idx, param] : m.params | std::views::enumerate) {
          if (idx > 0) { oss << ", "; }
          oss << constRef(param.type) << " " << param.name;
        }
        if (!m.params.empty()) oss << ", ";
        oss << "std::function<void(std::expected<" << serializeTypename(m.returnType)
            << ", std::string>)> cb";
        oss << "){\n";

        oss << "m_transport.request<" << serializeTypename(m.returnType) << ", "
            << getMethodParamName(s->name, m.name) << ">(" << std::quoted(methodId) << ", \n";
        oss << "{";

        for (const auto &[idx, p] : m.params | std::views::enumerate) {
          if (idx > 0) oss << ", ";
          oss << "." << p.name << " = " << p.name;
        }

        oss << "}, std::move(cb)\n";
        oss << ");\n}";
      }

      for (const auto &event : s->events) {
        oss << "\tvoid on" << toPascalCase(std::string{event.name}) << "(std::function<void(";
        for (const auto &[idx, param] : event.params | std::views::enumerate) {
          if (idx > 0) oss << ", ";
          oss << constRef(param.type);
        }
        oss << ")> handler) {\n";
        oss << "\t\tm_" << event.name << "Handler = std::move(handler);\n";
        oss << "\t}\n";
      }

      oss << "private:\n";
      oss << "RpcTransport& m_transport;\n";

      for (const auto &event : s->events) {
        oss << "std::function<void(";
        for (const auto &[idx, param] : event.params | std::views::enumerate) {
          if (idx > 0) oss << ", ";
          oss << constRef(param.type);
        }
        oss << ")> m_" << event.name << "Handler;\n";
      }

      oss << "};\n";
    }

    oss << "class Client {\n";
    oss << "\tpublic:\n";

    oss << "\tClient(RpcTransport& transport): m_transport(transport)";

    for (const auto &s : ast.services) {
      oss << ", m_" << s->name << "(transport)";
    }

    oss << " {}\n";

    oss << R"(
		std::expected<void, std::string> route(std::string_view data) {
			return m_transport.dispatchMessage(data);
		}
	)";

    for (const auto &s : ast.services) {
      oss << "\t\t" << s->name << "Service& " << toCamelCase(std::string{s->name}) << "() { return m_"
          << s->name << "; }\n";
    }

    oss << "\tprivate:\n";
    oss << "\t\tRpcTransport& m_transport;\n";

    for (const auto &s : ast.services) {
      oss << "\t\t" << s->name << "Service m_" << s->name << ";\n";
    }

    oss << "};\n";

    oss << "\n}"; // end namespace

    oss << serializeEnumGlazeMetas(ns, ast.enums);

    return oss.str();
  }

  std::string generateServer(const Tree &ast, const CodegenOptions &opts) override {
    std::ostringstream oss;

    oss << R"(
#pragma once
#include <expected>
#include <glaze/glaze.hpp>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
	)";

    auto const ns =
        opts.generationNamespace.value_or(std::string{stripExtension(opts.file.filename().string())});

    oss << "namespace " << ns << " {\n";
    oss << GLAZE_COMMON << GLAZE_SERVER_BASE;

    generateTypes(oss, ast);

    for (const auto &s : ast.services) {
      oss << serializeService(*s) << "\n";
    }

    oss << "class Server {\n";
    oss << "\tpublic:\n";
    oss << "\tServer(RpcTransport& transport";

    for (const auto &s : ast.services) {
      oss << ", " << abstractName(s->name) << "& " << s->name;
    }

    oss << "): m_transport(transport)";

    for (const auto &s : ast.services) {
      oss << ", m_" << s->name << "(" << s->name << ")";
    }

    oss << " {}\n";

    oss << R"(
	void route(std::string_view data) {
		JsonRpcRequest msg;
		if (auto const error = glz::read<glz::opts{.error_on_unknown_keys = false}>(msg, data)) { return; }
		if (msg.method.empty()) return;
		dispatch(msg);
	}
	)";

    for (const auto &s : ast.services) {
      oss << "\t" << abstractName(s->name) << "& " << toCamelCase(std::string{s->name}) << "() { return m_"
          << s->name << "; }\n";
    }

    oss << "private:\n";

    oss << "\tvoid dispatch(const JsonRpcRequest& req) {\n";

    for (const auto &s : ast.services) {
      for (const auto &m : s->methods) {
        std::string methodId = std::string{s->name} + "/" + m.name;

        oss << "\t\tif (req.method == " << std::quoted(methodId) << ") {\n";
        oss << "\t\t\t" << getMethodParamName(s->name, m.name) << " payload;\n";
        oss << "\t\t\t[[maybe_unused]] auto res = glz::read_json(payload, req.params.str);\n";
        oss << "\t\t\tauto result = m_" << s->name << "." << m.name << "(";
        for (const auto &[idx, param] : m.params | std::views::enumerate) {
          if (idx > 0) oss << ", ";
          oss << "std::move(payload." << param.name << ")";
        }
        oss << ");\n";

        oss << "\t\t\tif (result) {\n";
        oss << "\t\t\t\tm_transport.reply(req.id, *result);\n";
        oss << "\t\t\t} else {\n";
        oss << "\t\t\t\tm_transport.replyError(req.id, result.error());\n";
        oss << "\t\t\t}\n";
        oss << "\t\t\treturn;\n";
        oss << "\t\t}\n";
      }
    }

    oss << "\n\t}\n";

    oss << "\tRpcTransport& m_transport;\n";

    for (const auto &s : ast.services) {
      oss << "\t" << abstractName(s->name) << "& m_" << s->name << ";\n";
    }

    oss << "\n};\n";

    oss << "\n}"; // end namespace

    oss << serializeEnumGlazeMetas(ns, ast.enums);

    return oss.str();
  }
};

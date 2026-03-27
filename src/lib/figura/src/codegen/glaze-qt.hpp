#pragma once
#include "codegen.hpp"
#include <format>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <variant>
#include <ranges>

constexpr const auto BASE = R"(
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
  std::string jsonrpc;
  std::string method;
  int id;
  glz::raw_json result;
};

using IncomingJsonRpcMessage = std::variant<JsonRpcRequest, JsonRpcNotification>;
using OutgoingJsonRpcMessage = std::variant<JsonRpcResponse, JsonRpcErrorResponse>;

class AbstractTransport {
public:
  using MessageHandler = std::function<void(const IncomingJsonRpcMessage &)>;
  virtual void sendResponse(std::string_view data) = 0;
  virtual ~AbstractTransport() = default;
};

class EventEmitter {
	public:
		EventEmitter(AbstractTransport& transport): m_transport(transport) {
		}

		template <typename T>
		void emitEvent(std::string_view name, const T& params) {
			std::string paramsBuf;
			{
			[[maybe_unused]] auto res = glz::write_json(params, paramsBuf);
			}

			JsonRpcNotification notif{
				.method = std::string{name},
				.params = paramsBuf
			};

			std::string buf;
			{
			[[maybe_unused]] auto res = glz::write_json(notif, buf);
			}

			m_transport.sendResponse(buf);
		}
	
	private:
		AbstractTransport& m_transport;
};

template <typename T, typename U>
concept DerivedFrom = std::derived_from<T, U>;
)";

class GlazeQtGenerator : public AbstractCodeGenerator {
  std::string name() const override { return "glaze-qt"; }

  static std::string_view serializeTypenameImpl(const TypeValue &value) {
    auto const visitor = overloads{[](TypeStruct *s) { return s->name; },
                                   [](const EnumValue &e) { return std::string_view{"std::string"}; },
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

  static std::string serializeEnum(const EnumValue &e) {
    std::ostringstream oss;

    oss << "enum class " << e.name << "{\n";
    for (const auto &v : e.values) {
      oss << "\t" << v << ",\n";
    }

    oss << "\n};\n";

    return oss.str();
  }

  static std::string serializeService(const Service &s) {
    std::ostringstream oss;

    oss << "template <typename UserContext>\n";
    oss << "class " << abstractName(s.name) << ": public EventEmitter {\n";
    oss << "\tpublic:\n";
    oss << "\t" << abstractName(s.name)
        << "(UserContext ctx, AbstractTransport& transport): EventEmitter(transport), m_ctx(ctx) {}\n\n";

    for (const auto &method : s.methods) {
      oss << "\t" << "virtual " << "QFuture<" << serializeTypename(method.returnType) << "> " << method.name
          << "(";
      for (const auto &[idx, param] : method.params | std::views::enumerate) {
        if (idx > 0) oss << ", ";
        oss << constRef(param.type) << " " << param.name;
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

    oss << "protected:\n";
    oss << "\tUserContext m_ctx;\n";

    oss << "\n};\n";

    return oss.str();
  }

  // <name>[?]:<type>
  static void serializeTypeExpression(std::ofstream &ofs, const TypeStruct &s) {}

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

public:
  std::string generateClient(const Tree &ast) override { throw std::runtime_error("not implemented"); }

  std::string generateServer(const Tree &ast) override {
    std::ostringstream oss;

    oss << R"(
#pragma once
#include <QFuture>
#include <memory>
#include <glaze/glaze.hpp>
#include <glaze/json/read.hpp>
#include <concepts>
#include <variant>
#include <string_view>
	)";

    oss << "namespace codegen {\n";
    oss << BASE;

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

      oss << serializeService(*s) << "\n";
    }

    oss << "template <\n";
    oss << "\ttypename UserContext,\n";
    for (const auto &[idx, s] : ast.services | std::views::enumerate) {
      if (idx > 0) oss << ", ";
      // oss << "\tDerivedFrom<" << abstractName(s->name) << "<UserContext>> " << s->name << "\n";
      oss << "\tclass " << " " << s->name << "\n";
    }

    oss << ">\n";
    oss << "class Server {\n";
    oss << "\tpublic:\n";
    oss << "\tServer(UserContext ctx, AbstractTransport* transport): m_ctx(ctx), m_transport(transport)";

    for (const auto &s : ast.services) {
      oss << ", ";
      oss << "m_" << s->name << "(" << "new " << s->name << "(ctx, *transport)" << ")";
    }

    oss << "{\n";
    oss << "\n}\n";

    oss << R"(
	void route(std::string_view data) {
		IncomingJsonRpcMessage msg;
		[[maybe_unused]] auto res = glz::read_json(msg, data);
		if (auto reqPtr = std::get_if<JsonRpcRequest>(&msg)) { dispatch(*reqPtr); }
	}
	)";

    oss << "private:\n";

    oss << R"(
	void sendResponse(const JsonRpcResponse& res) {
		std::string buf;
    	[[maybe_unused]] auto result = glz::write_json(res, buf);
		m_transport->sendResponse(buf);
	}

	void reply(int id, const auto &data) {
    	static thread_local std::string buf;
    	[[maybe_unused]]auto result = glz::write_json(data, buf);
    	sendResponse(JsonRpcResponse{.id = id, .result = buf});
  	}

  	void replyEmpty(int id) { sendResponse(JsonRpcResponse{.id = id, .result = "{}"}); }
	)";

    oss << "\tvoid dispatch(const JsonRpcRequest& req) {\n";

    for (const auto &s : ast.services) {
      oss << "\t\t" << "// " << s->name << "\n";
      for (const auto &m : s->methods) {
        std::string methodId = std::string{s->name} + "/" + m.name;

        oss << "\t\tif (req.method == " << std::quoted(methodId) << ") {\n";
        oss << "\t\t\t" << getMethodParamName(s->name, m.name) << " payload;\n";
        oss << "\t\t\t[[maybe_unused]] auto res = glz::read_json(payload, req.params.str);\n";
        oss << "\t\t\t" << "m_" << s->name << "->" << m.name << "(";
        for (const auto &[idx, param] : m.params | std::views::enumerate) {
          if (idx > 0) oss << ", ";
          oss << "payload." << param.name;
        }

        if (auto tt = std::get_if<PrimitiveType>(&m.returnType.data); tt && *tt == PrimitiveType::Void) {
          oss << ").then([this, id = req.id](){ replyEmpty(id); });";
        } else {
          oss << ").then([this, id = req.id](" << constRef(m.returnType) << " res){ reply(id, res); });";
        }

        oss << "\n\t\t}\n";
      }
    }

    oss << "\n\t}\n";

    for (const auto &s : ast.services) {
      oss << "\t" << "std::unique_ptr<" << abstractName(s->name) << "<UserContext>> " << "m_" << s->name
          << ";\n";
    }

    oss << "\tUserContext m_ctx;\n";
    oss << "\tAbstractTransport* m_transport;\n";
    oss << "\n";

    oss << "\n};\n";

    oss << "\n}"; // end namespace

    return oss.str();
  }
};

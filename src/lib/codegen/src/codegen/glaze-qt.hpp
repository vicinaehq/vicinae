#pragma once
#include "codegen.hpp"
#include <iomanip>
#include <sstream>
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
  virtual void onMessage(const MessageHandler &message) = 0;
  virtual void sendResponse(const JsonRpcResponse& res) = 0;
  virtual void sendNotification(const JsonRpcNotification& notif) = 0;
  virtual ~AbstractTransport() = default;
};

template <typename T, typename U>
concept DerivedFrom = std::derived_from<T, U>;
)";

class GlazeQtGenerator : public AbstractCodeGenerator {
  std::string name() const override { return "glaze-qt"; }

  static std::string serializeTypename(const TypeValue &value) {
    if (auto ptr = std::get_if<TypeStruct *>(&value.data)) {
      return std::string{(*ptr)->name};
    } else {
      return "std::string";
    }
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
    oss << "class " << abstractName(s.name) << " {\n";
    oss << "\t" << abstractName(s.name) << "(UserContext ctx): m_ctx(ctx) {}\n\n";

    for (const auto &method : s.methods) {
      oss << "\t" << "virtual " << serializeTypename(method.returnType) << " " << method.name << "(";
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
      oss << ") {}\n";
    }

    oss << "protected:\n";
    oss << "\tUserContext m_ctx;\n";

    oss << "\n};\n";

    return oss.str();
  }

  static std::string serializeType(const TypeStruct &s) {
    std::ostringstream oss;

    oss << "struct " << s.name << " {\n";

    for (const auto &field : s.fields) {
      oss << "\t" << serializeTypename(field.type) << " " << field.name << ";\n";
    }

    oss << "\n};\n";

    return oss.str();
  }

  static std::string getMethodParamName(std::string_view mname) { return std::string{mname} + "Params"; }

  static std::string serializeMethodParams(const Method &method) {
    std::ostringstream oss;

    oss << "struct " << getMethodParamName(method.name) << " {\n";

    for (const auto &param : method.params) {
      oss << "\t" << serializeTypename(param.type) << " " << param.name << ";\n";
    }

    oss << "\n};\n";

    return oss.str();
  }

public:
  std::string generate(const Tree &ast) override {
    std::ostringstream oss;

    oss << R"(
#pragma once
#include <memory>
#include <glaze/glaze.hpp>
#include <glaze/json/read.hpp>
#include <qfuture.h>
#include <concepts>
#include <variant>
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
      oss << serializeService(*s) << "\n";

      for (const auto &m : s->methods) {
        oss << serializeMethodParams(m);
      }
    }

    oss << "template <\n";
    oss << "\ttypename UserContext,\n";
    for (const auto &[idx, s] : ast.services | std::views::enumerate) {
      if (idx > 0) oss << ", ";
      oss << "\tDerivedFrom<" << abstractName(s->name) << "<UserContext>> " << s->name << "\n";
    }

    oss << ">\n";
    oss << "class Server {\n";
    oss << "\tServer(UserContext ctx, AbstractTransport* transport): m_ctx(ctx), m_transport(transport)";

    for (const auto &s : ast.services) {
      oss << ", ";
      oss << s->name << "(" << "new " << s->name << "(ctx)" << ")";
    }

    oss << "{\n";
    oss << "\t m_transport->onMessage([this](auto message){ if (auto reqPtr = "
           "std::get_if<JsonRpcRequest>(&message)) { dispatch(*reqPtr); } });";
    oss << "\n}\n";

    oss << "private:\n";

    oss << R"(

	template <glz::reflectable T> 
	void reply(int id, const T &data) {
    	static thread_local std::string buf;
    	[[maybe_unused]]auto result = glz::write_json(data, buf);

    	m_transport->sendResponse(JsonRpcResponse{.id = id, .result = result});
  	}

	)";

    oss << "\tvoid dispatch(const JsonRpcRequest& req) {\n";

    for (const auto &s : ast.services) {
      oss << "\t\t" << "// " << s->name << "\n";
      for (const auto &m : s->methods) {
        std::string methodId = std::string{s->name} + "/" + m.name;

        oss << "\t\tif (req.method == " << std::quoted(methodId) << ") {\n";
        oss << "\t\t\t" << getMethodParamName(m.name) << " payload;\n";
        oss << "\t\t\t[[maybe_unused]] auto res = glz::read_json(payload, req.params.str);\n";
        oss << "\t\t\t" << "m_" << s->name << "->" << m.name << "(";
        for (const auto &[idx, param] : m.params | std::views::enumerate) {
          if (idx > 0) oss << ", ";
          oss << "payload." << param.name;
        }
        oss << ").then([this, id = req.id](auto res){ reply(id, res); });\n";
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

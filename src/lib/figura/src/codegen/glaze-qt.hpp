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

constexpr const auto COMMON = R"(
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

using IncomingJsonRpcMessage = std::variant<JsonRpcRequest, JsonRpcNotification>;
using OutgoingJsonRpcMessage = std::variant<JsonRpcResponse, JsonRpcNotification, JsonRpcErrorResponse>;

class AbstractTransport {
public:
  virtual void send(std::string_view data) = 0;
  virtual ~AbstractTransport() = default;
};
)";

constexpr const auto clientCode = R"(
export class RpcTransport {
	RpcTransport(AbstractTransport& transport): m_transport(transport) {}

	void dispatchMessage(data: string) {
		const msg = JSON.parse(data) as JsonRpcMessage;
		IncomingJsonRpcMessage msg;

		[[maybe_unused]] auto res = glz::read_json(msg, data);

		if (auto req = std::get_if<JsonRpcRequest>(&msg)) {
			if (auto it = m_handlers.find(req->id); it != m_handlers.end()) {
			}
		}
		

		if (msg.id) {
			const handler = this.requestMap.get(msg.id);

			if (handler) {
				if (msg.error) handler.reject(msg.error);
				if (msg.result) handler.resolve(msg.result);
				this.requestMap.delete(msg.id);
			}
		}
		if (!msg.id && msg.method) {
			for (const cb of this.handlers.get(msg.method) ?? []) {
				cb(msg.params);
			}
		}
	}

	request<T>(method: string, params: Record<string, any>): Promise<T> {
		const id = this.id++;
		const promise = new Promise<T>((resolve, reject) => {
			this.requestMap.set(id, { resolve: (msg) => resolve(msg as T), reject });
		});

		this.sendMessage({ jsonrpc: '2.0', id, method, params });

		return promise;
	}

	subscribe(method: string, cb: (result: any) => void): EventSubscription {
		const handlers = this.handlers.get(method);

		if (handlers) handlers.push(cb);
		else this.handlers.set(method, [cb]);

		return {
			unsubscribe: () => {
				const handlers = this.handlers.get(method);

				if (handlers) {
					handlers.splice(handlers.indexOf(cb), 1);
				}
			},
		};
	}

 	sendMessage(msg: JsonRpcMessage) {
		this.transport.send(JSON.stringify(msg));
	}

	using Handler = std::function<void(std::string_view result)>;

	int id = 0;
	std::unordered_map<std::string, std::vector<Handler>> m_handlers;
	AbstractTransport& m_transport;
};
)";

constexpr const auto BASE = R"(


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
				.result = resultBuf
			});
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
        << "(UserContext ctx, RpcTransport& transport): EventEmitter(transport), m_ctx(ctx) {}\n\n";

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

  static std::string_view stripExtension(std::string_view path) {
    auto const pos = path.find_last_of('.');
    if (pos != std::string_view::npos) return path.substr(0, pos);
    return path;
  }

public:
  std::string generateClient(const Tree &ast, const CodegenOptions &opts) override {
    throw std::runtime_error("not implemented");
  }

  std::string generateServer(const Tree &ast, const CodegenOptions &opts) override {
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

    auto const ns =
        opts.generationNamespace.value_or(std::string{stripExtension(opts.file.filename().string())});

    oss << "namespace " << ns << " {\n";
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
          oss << ").then([this, id = req.id](){ m_transport->reply(id, nullptr); });";
        } else {
          oss << ").then([this, id = req.id](" << constRef(m.returnType)
              << " res){ m_transport->reply(id, res); });";
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
    oss << "\tRpcTransport* m_transport;\n";
    oss << "\n";

    oss << "\n};\n";

    oss << "\n}"; // end namespace

    return oss.str();
  }
};

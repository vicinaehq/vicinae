#include "../parser.hpp"
#include <format>
#include "codegen.hpp"
#include <iomanip>
#include <sstream>
#include <ranges>
#include <stdexcept>

namespace {

inline std::string_view getTypename(const TypeValue &type) {
  if (auto ptr = std::get_if<TypeStruct *>(&type.data)) { return (*ptr)->name; }
  if (auto ptr = std::get_if<EnumValue>(&type.data)) { return ptr->name; }
  if (auto ptype = std::get_if<PrimitiveType>(&type.data)) {
    switch (*ptype) {
    case PrimitiveType::Void:
      return "void";
    case PrimitiveType::Boolean:
      return "boolean";
    case PrimitiveType::Number:
      return "number";
    case PrimitiveType::String:
      return "string";
    }
  }

  return "string";
}

inline std::string getTypeSignature(const TypeValue &type) {
  std::string str{getTypename(type)};
  if (type.isArray) str.append("[]");
  return str;
}

inline std::string generateType(const TypeStruct &s) {
  std::ostringstream oss;
  oss << "type " << s.name << " = {\n";

  for (const auto &field : s.fields) {
    oss << "\t" << field.name;

    if (field.type.isOptional) oss << "?";

    oss << ": " << getTypeSignature(field.type) << ";\n";
  }

  oss << "}";

  return oss.str();
}

inline std::string generateEnum(const EnumValue &e) {
  std::ostringstream oss;
  oss << "type " << e.name << " = ";

  for (const auto &[idx, value] : e.values | std::views::enumerate) {
    if (idx > 0) oss << " | ";
    oss << "'" << value << "'";
  }

  oss << ";";

  return oss.str();
}

inline std::string generateEventHandlerTypeSignature(const Event &event) {
  std::ostringstream oss;
  oss << "(";
  for (const auto &[idx, param] : event.params | std::views::enumerate) {
    if (idx > 0) oss << ", ";
    oss << param.name << ": " << getTypeSignature(param.type);
  }
  oss << ") => void";
  return oss.str();
}

inline std::string generateService(const Service &s) {
  std::ostringstream oss;
  oss << "class " << s.name << " {\n";

  oss << "\tconstructor(private readonly transport: ClientTransport) {}\n\n";

  for (const auto &method : s.methods) {
    oss << "\t" << method.name << "(";

    for (const auto &[idx, param] : method.params | std::views::enumerate) {
      if (idx > 0) oss << ", ";
      oss << param.name;
      if (param.type.isOptional) oss << "?";
      oss << ": " << getTypeSignature(param.type);
    }

    oss << "): " << "Promise<" << getTypeSignature(method.returnType) << "> {\n";

    std::string methodName = std::format("{}/{}", s.name, method.name);

    oss << "\t\treturn this.transport.request(" << std::quoted(methodName) << ", { ";

    for (const auto &[idx, param] : method.params | std::views::enumerate) {
      if (idx > 0) oss << ", ";
      oss << param.name;
    }

    oss << "});";
    oss << "\t" << "\n\t}\n\n";
  }

  if (!s.events.empty()) {
    for (const auto &event : s.events) {
      std::string eventName = std::format("{}/{}", s.name, event.name);

      oss << "\t" << event.name << "(handler: " << generateEventHandlerTypeSignature(event)
          << "): EventSubscription {\n";
      oss << "\t\treturn this.transport.subscribe(" << std::quoted(eventName) << ", (msg) => handler(";

      for (const auto &[idx, param] : event.params | std::views::enumerate) {
        if (idx > 0) oss << ", ";
        oss << "msg." << param.name;
      }

      oss << "))";
      oss << "\n\t}\n";
    }
  }

  oss << "}";

  return oss.str();
}

}; // namespace

static constexpr const auto busCode = R"(
interface JsonRpcMessage {
	jsonrpc: "2.0";
	method?: string;
	id?: number;
	params?: Record<string, any>;
	result?: any;
	error?: any;
}

interface ITransport {
	send(data: string): void;
}

type EventSubscription = {
	unsubscribe: () => void;
};

class ClientTransport {
	constructor(private readonly transport: ITransport) { }

	dispatchMessage(data: string) {
		const msg = JSON.parse(data) as JsonRpcMessage;

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
		this.handlers[method] = cb;

		return {
			unsubscribe: () => {
				const handlers = this.handlers[method];

				if (handlers) {
					handlers.splice(handlers.indexOf(cb), 1);
				}
			}
		}
	}

	private sendMessage(msg: JsonRpcMessage) {
		this.transport.send(JSON.stringify(msg));
	}


	private id = 0;
	private requestMap = new Map<number, { resolve: (value: any) => void, reject: (error: any) => void }>;
	private handlers = new Map<string, Array<(result: any) => void>>;
};
)";

static std::string tab(int n) {
  std::string str;
  str.reserve(n);
  for (int i = 0; i != n; ++i)
    str += '\t';
  return str;
}

static std::string codegenTypescript(const Tree &tree) {
  std::ostringstream oss;

  oss << "// This file was generated by vicinae codegen: do not edit manually.\n\n";
  oss << busCode << "\n";

  for (auto const &s : tree.enums) {
    oss << generateEnum(s) << "\n\n";
  }

  for (auto const &s : tree.structs) {
    oss << generateType(*s) << "\n\n";
  }

  for (auto const &s : tree.services) {
    oss << generateService(*s) << "\n\n";
  }

  oss << "class Client {\n";
  oss << "\tconstructor(transport: ITransport) {\n";

  oss << tab(2) << "this.transport = new ClientTransport(transport);\n";

  for (auto const &s : tree.services) {
    oss << "\t\tthis." << s->name << " = new " << s->name << "(this.transport);\n";
  }

  oss << "\t}\n";

  oss << R"(
  	route(msg: string): void { this.transport.dispatchMessage(msg); }
  )";

  for (auto const &s : tree.services) {
    oss << "\t" << s->name << ": " << s->name << ";\n";
  }

  oss << "\t" << "private transport: ClientTransport;\n";

  oss << "\n}\n";

  return oss.str();
}

class TypeScriptCodeGenerator : public AbstractCodeGenerator {
  std::string name() const override { return "typescript"; }
  std::string generateClient(const Tree &ast) override { return codegenTypescript(ast); }
  std::string generateServer(const Tree &ast) override { throw std::runtime_error("generate server"); }
};

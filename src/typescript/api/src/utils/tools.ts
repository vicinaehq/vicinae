import { existsSync, readFileSync, writeFileSync } from "node:fs";
import { join } from "node:path";
import * as ts from "typescript";
import { parseConfig } from "./typecheck.js";

type JsonSchema = { [key: string]: unknown };

type ToolInfo = {
	instructions?: string;
	input?: JsonSchema;
	output?: JsonSchema;
	confirmation: boolean;
};

type ManifestTool = { name: string };

const SOURCE_EXTENSIONS = [".ts", ".tsx", ".js", ".jsx"];

class SchemaError extends Error {}

export const toolSource = (root: string, name: string) =>
	SOURCE_EXTENSIONS.map((ext) =>
		join(root, "src", "tools", `${name}${ext}`),
	).find(existsSync);

export const toolEntryPoints = (root: string, tools: ManifestTool[]) =>
	tools.map((tool) => {
		const source = toolSource(root, tool.name);
		if (!source) {
			throw new Error(
				`could not find entrypoint src/tools/${tool.name}.{ts,tsx,js,jsx} for tool ${tool.name}.`,
			);
		}
		return { in: source, out: join("tools", tool.name) };
	});

const documentation = (
	checker: ts.TypeChecker,
	symbol: ts.Symbol | ts.Signature | undefined,
) => {
	if (!symbol) return undefined;
	const text = ts.displayPartsToString(symbol.getDocumentationComment(checker));
	const tags = symbol
		.getJsDocTags()
		.map((tag) =>
			[`@${tag.name}`, ts.displayPartsToString(tag.text)]
				.filter(Boolean)
				.join(" "),
		);
	if (!text && tags.length === 0) return undefined;
	return tags.length > 0 ? `${text}\n\n${tags.join("\n")}` : text;
};

class SchemaConverter {
	private readonly visiting = new Set<ts.Type>();

	constructor(private readonly checker: ts.TypeChecker) {}

	convert(type: ts.Type, symbol?: ts.Symbol): JsonSchema {
		if (this.visiting.has(type)) {
			throw new SchemaError("Recursive types are not supported");
		}
		this.visiting.add(type);
		try {
			return this.convertType(type, symbol);
		} finally {
			this.visiting.delete(type);
		}
	}

	private convertType(type: ts.Type, symbol?: ts.Symbol): JsonSchema {
		const checker = this.checker;
		const description = documentation(
			checker,
			symbol ?? type.aliasSymbol ?? type.symbol,
		);
		const fail = (message: string): never => {
			throw new SchemaError(
				symbol ? `${message} for "${symbol.name}"` : message,
			);
		};

		if (type.flags & ts.TypeFlags.Any) {
			return fail("`any` type is not supported");
		}
		if (type.flags & ts.TypeFlags.Unknown) {
			return fail("`unknown` type is not supported");
		}
		if (type.flags & ts.TypeFlags.StringLike && !type.isStringLiteral()) {
			return { type: "string", description };
		}
		if (type.flags & ts.TypeFlags.NumberLike && !type.isNumberLiteral()) {
			return { type: "number", description };
		}
		if (type.flags & ts.TypeFlags.BooleanLike) {
			return { type: "boolean", description };
		}
		if (type.isStringLiteral()) {
			return { type: "string", description, enum: [type.value] };
		}

		if (type.isUnion()) {
			const members = type.types.filter(
				(member) =>
					!(member.flags & (ts.TypeFlags.Undefined | ts.TypeFlags.Null)),
			);
			if (members.length === 1) return this.convert(members[0], symbol);
			if (
				members.every((member) => member.flags & ts.TypeFlags.BooleanLiteral)
			) {
				return { type: "boolean", description };
			}
			if (members.every((member) => member.isStringLiteral())) {
				return {
					type: "string",
					description,
					enum: members.map((member) => (member as ts.StringLiteralType).value),
				};
			}
			return fail("Only string unions are supported");
		}

		if (type.isIntersection()) {
			return fail("Intersection types are not supported");
		}

		if (checker.isArrayType(type)) {
			const [item] = checker.getTypeArguments(type as ts.TypeReference);
			if (!item) return fail("No item type");
			return { type: "array", items: this.convert(item), description };
		}

		if (type.symbol?.name === "Date") {
			return { type: "string", description };
		}

		if (type.flags & ts.TypeFlags.Object) {
			if (type.getCallSignatures().length > 0) {
				return fail("Functions are not supported");
			}
			if (checker.isTupleType(type)) {
				return fail("Tuples are not supported");
			}

			const schema: JsonSchema = {
				type: "object",
				properties: {},
				required: [],
				description,
			};
			const properties = schema.properties as Record<string, JsonSchema>;
			const required = schema.required as string[];

			for (const property of checker.getPropertiesOfType(type)) {
				const declaration = property.valueDeclaration;
				if (
					declaration &&
					(ts.isMethodDeclaration(declaration) ||
						ts.isGetAccessor(declaration) ||
						ts.isSetAccessor(declaration))
				) {
					continue;
				}
				const propertyType = declaration
					? checker.getTypeOfSymbolAtLocation(property, declaration)
					: checker.getTypeOfSymbol(property);
				properties[property.name] = this.convert(propertyType, property);
				if (!(property.flags & ts.SymbolFlags.Optional))
					required.push(property.name);
			}

			const index = checker.getIndexTypeOfType(type, ts.IndexKind.String);
			if (index) schema.additionalProperties = this.convert(index);

			return schema;
		}

		return fail(`Unsupported type ${checker.typeToString(type)}`);
	}

	/**
	 * Output schemas are informative only, so anything we cannot describe is simply left out.
	 */
	tryConvert(type: ts.Type): JsonSchema | undefined {
		if (
			type.flags &
			(ts.TypeFlags.Void | ts.TypeFlags.Undefined | ts.TypeFlags.Never)
		) {
			return undefined;
		}
		try {
			return this.convert(type);
		} catch (error) {
			if (error instanceof SchemaError) return undefined;
			throw error;
		}
	}
}

const extractTool = (program: ts.Program, file: string): ToolInfo => {
	const checker = program.getTypeChecker();
	const source = program.getSourceFile(file);
	const moduleSymbol = source && checker.getSymbolAtLocation(source);
	if (!moduleSymbol) throw new Error("Could not load the tool's source file");

	const exports = checker.getExportsOfModule(moduleSymbol);
	let defaultExport = exports.find((symbol) => symbol.name === "default");
	if (!defaultExport) throw new Error("Default exported function not found");
	if (defaultExport.flags & ts.SymbolFlags.Alias) {
		defaultExport = checker.getAliasedSymbol(defaultExport);
	}

	const [signature] = checker
		.getTypeOfSymbolAtLocation(defaultExport, source)
		.getCallSignatures();
	if (!signature) throw new Error("The default export is not a function");

	const parameters = signature.getParameters();
	if (parameters.length > 1) {
		throw new Error(
			`Tools should have at most one parameter (has ${parameters.length})`,
		);
	}

	const converter = new SchemaConverter(checker);
	const [parameter] = parameters;
	const input = parameter
		? converter.convert(
				checker.getTypeOfSymbolAtLocation(parameter, source),
				parameter,
			)
		: undefined;
	const returnType = signature.getReturnType();
	const output = converter.tryConvert(
		checker.getAwaitedType(returnType) ?? returnType,
	);

	return {
		instructions: documentation(checker, signature),
		input,
		output,
		confirmation: exports.some((symbol) => symbol.name === "confirmation"),
	};
};

export type ToolExtraction = {
	tools: Map<string, ToolInfo>;
	errors: string[];
};

export const extractTools = (
	root: string,
	tools: ManifestTool[],
): ToolExtraction => {
	const result: ToolExtraction = { tools: new Map(), errors: [] };
	const sources = tools.flatMap((tool) => {
		const file = toolSource(root, tool.name);
		if (!file) result.errors.push(`tool ${tool.name}: source file not found`);
		return file ? [{ name: tool.name, file }] : [];
	});

	if (sources.length === 0) return result;

	const parsed = parseConfig(root);
	const envFile = join(root, "vicinae-env.d.ts");
	const program = ts.createProgram(
		[
			...parsed.fileNames,
			...(existsSync(envFile) ? [envFile] : []),
			...sources.map((source) => source.file),
		],
		{
			...parsed.options,
			noEmit: true,
			allowJs: true,
			jsx: ts.JsxEmit.ReactJSX,
			esModuleInterop: true,
		},
	);

	for (const { name, file } of sources) {
		try {
			result.tools.set(name, extractTool(program, file));
		} catch (error) {
			result.errors.push(
				`tool ${name}: ${error instanceof Error ? error.message : String(error)}`,
			);
		}
	}

	return result;
};

export const writeBuiltManifest = (
	root: string,
	outDir: string,
	extraction: ToolExtraction,
) => {
	if (extraction.errors.length > 0) {
		throw new Error(
			`Failed to generate tool schemas:\n${extraction.errors.join("\n")}`,
		);
	}
	const pkg = JSON.parse(readFileSync(join(root, "package.json"), "utf8"));

	if (Array.isArray(pkg.tools)) {
		pkg.tools = pkg.tools.map((tool: ManifestTool) => {
			const info = extraction.tools.get(tool.name);
			if (!info) throw new Error(`Missing metadata for tool ${tool.name}`);
			return { ...tool, ...info };
		});
	}

	writeFileSync(join(outDir, "package.json"), JSON.stringify(pkg, null, 2));
};

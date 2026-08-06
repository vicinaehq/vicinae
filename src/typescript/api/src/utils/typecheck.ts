import * as path from "node:path";
import * as ts from "typescript";

export type TypeCheckResult = {
	ok: boolean;
	output: string;
};

const FORCED_OPTIONS: ts.CompilerOptions = {
	noEmit: true,
	jsx: ts.JsxEmit.ReactJSX,
	esModuleInterop: true,
	skipLibCheck: true,
};

const DEFAULT_CONFIG = {
	compilerOptions: {
		target: "ES2022",
		module: "commonjs",
		moduleResolution: "node",
		strict: true,
		resolveJsonModule: true,
	},
	include: ["src"],
};

const parseConfig = (target: string): ts.ParsedCommandLine => {
	const configPath = ts.findConfigFile(target, ts.sys.fileExists);

	if (configPath) {
		const parsed = ts.getParsedCommandLineOfConfigFile(
			configPath,
			{ noEmit: true },
			{
				...ts.sys,
				onUnRecoverableConfigFileDiagnostic: (diagnostic) => {
					throw new Error(
						ts.flattenDiagnosticMessageText(diagnostic.messageText, "\n"),
					);
				},
			},
		);
		if (parsed) return parsed;
	}

	return ts.parseJsonConfigFileContent(DEFAULT_CONFIG, ts.sys, target);
};

export const typeCheck = (target: string): TypeCheckResult => {
	const parsed = parseConfig(target);
	const envFile = path.join(target, "vicinae-env.d.ts");
	const rootNames = ts.sys.fileExists(envFile)
		? [...parsed.fileNames, envFile]
		: parsed.fileNames;
	const program = ts.createProgram(rootNames, {
		...parsed.options,
		...FORCED_OPTIONS,
	});
	const diagnostics = [
		...ts.getConfigFileParsingDiagnostics(parsed),
		...ts.getPreEmitDiagnostics(program),
	];
	const output = ts.formatDiagnosticsWithColorAndContext(diagnostics, {
		getCurrentDirectory: () => target,
		getCanonicalFileName: (fileName) => fileName,
		getNewLine: () => ts.sys.newLine,
	});

	return {
		ok: !diagnostics.some(
			(diagnostic) => diagnostic.category === ts.DiagnosticCategory.Error,
		),
		output,
	};
};

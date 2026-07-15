import * as ts from "typescript";

export type TypeCheckResult = {
	ok: boolean;
	output: string;
};

const DEFAULT_CONFIG = {
	compilerOptions: {
		target: "ES2022",
		module: "commonjs",
		moduleResolution: "node",
		jsx: "react-jsx",
		strict: true,
		esModuleInterop: true,
		skipLibCheck: true,
		resolveJsonModule: true,
	},
	include: ["src", "vicinae-env.d.ts"],
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
	const program = ts.createProgram(parsed.fileNames, {
		...parsed.options,
		noEmit: true,
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

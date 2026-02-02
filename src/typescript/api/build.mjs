import * as esbuild from "esbuild";

await esbuild.build({
	entryPoints: ["src/**/*.ts", "src/**/*.tsx"],
	jsx: "automatic",
	platform: "node",
	outdir: "dist",
});

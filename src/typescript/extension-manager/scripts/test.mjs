import * as esbuild from "esbuild";
import { spawnSync } from "node:child_process";
import { mkdtempSync, readdirSync, rmSync } from "node:fs";
import { tmpdir } from "node:os";
import { join } from "node:path";

const root = join(import.meta.dirname, "..");
const outDir = mkdtempSync(join(tmpdir(), "vicinae-extension-manager-test-"));
const entryPoints = readdirSync(join(root, "test"))
	.filter((f) => f.endsWith(".test.tsx") || f.endsWith(".test.ts"))
	.map((f) => join(root, "test", f));

let status = 1;
try {
	await esbuild.build({
		entryPoints,
		bundle: true,
		outdir: outDir,
		format: "cjs",
		platform: "node",
		outExtension: { ".js": ".cjs" },
		alias: {
			"@vicinae/api": join(root, "../api/src/"),
			react: join(root, "../api/node_modules/react"),
		},
	});

	// a fixed non-UTC zone: date tests pass by accident when local time equals UTC
	const res = spawnSync(process.execPath, ["--test", outDir], {
		stdio: "inherit",
		env: { ...process.env, TZ: "America/Los_Angeles" },
	});
	status = res.status ?? 1;
} finally {
	rmSync(outDir, { recursive: true, force: true });
}
process.exit(status);

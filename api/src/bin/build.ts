import * as esbuild from 'esbuild';
import { readFileSync } from 'fs';
import { join } from 'path';

export const buildExtension = async (path: string = process.cwd()) => {
	const pkgPath = join(path, "package.json");
	const pkg = JSON.parse(readFileSync(pkgPath, 'utf-8'));
	const outdir = join(path, "dist");

	for (const cmd of pkg.commands) {
		const source = join(path, 'src', `${cmd.name}.tsx`);

		await esbuild.build({
		  entryPoints: [source],
		  bundle: true,
		  external: [
			  "react",
			  "@omnicast/api"
		  ],
		  outfile: join(outdir, `${cmd.name}.js`),
		  format: 'cjs',
		  platform: 'node'
		});
	}
}

buildExtension(process.argv[2] ?? process.cwd());

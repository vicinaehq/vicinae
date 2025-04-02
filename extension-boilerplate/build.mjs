import * as esbuild from 'esbuild'
import { readFileSync } from 'fs';
import { join } from 'path';

const pkg = JSON.parse(readFileSync('package.json', 'utf-8'));

for (const cmd of pkg.commands) {
	const source = join('src', `${cmd.name}.tsx`);

	console.log(`Compiling command ${source}...`);
	await esbuild.build({
	  entryPoints: [source],
	  bundle: true,
	  external: [
		  "react",
		  "@omnicast/api"
	  ],
	  outfile: join('dist', `${cmd.name}.js`),
	  format: 'cjs',
	  platform: 'node'
	});
	console.log('Compilation successful.');
}

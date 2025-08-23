import * as esbuild from 'esbuild'
import { mkdirSync, rmSync } from 'fs'
import { join } from 'path';

const outDir = join(import.meta.dirname, '..', 'dist');
const outFile = join(outDir, 'runtime.js');

rmSync(outDir, { recursive: true, force: true });
mkdirSync(outDir, { recursive: true });

const start = performance.now();

console.log(`Building runtime...`);

await esbuild.build({
  entryPoints: ['src/index.ts'],
  bundle: true,
  outfile: outFile,
  format: 'cjs',
  external: [], 
  minify: true,
  platform: 'node',
	alias: {
		'@vicinae/api': '../api/src/', // we bundle the local api directly
		'react': '../api/node_modules/react' // we want the react version specified in the api package
	}
})

const end = performance.now();
const buildTimeMs = end - start;

console.log(`Runtime built at ${outFile} in ${Math.round(buildTimeMs)}ms 🚀`);

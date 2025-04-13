import * as esbuild from 'esbuild'
import { rmSync } from 'fs'
import { join } from 'path';

const OUT_DIR = 'dist';

rmSync(OUT_DIR, { recursive: true });

const start = performance.now();

console.log(`Building runtime...`);

await esbuild.build({
  entryPoints: ['src/index.ts'],
  bundle: true,
  outfile: join(OUT_DIR, 'runtime.js'),
  format: 'cjs',
  minify: true,
  platform: 'node',
	alias: {
		'@omnicast/api': '../api/src/',
		// we want react to always resolve to the local node_modules version
		'react': './node_modules/react'
	}
})

const end = performance.now();
const buildTimeMs = end - start;

console.log(`Runtime built in ${Math.round(buildTimeMs)}ms`);

import * as esbuild from 'esbuild'
import { copyFileSync, existsSync, rmSync } from 'fs'
import { join } from 'path';

const OUT_DIR = 'dist';

if (existsSync(OUT_DIR)) rmSync(OUT_DIR, { recursive: true });

const start = performance.now();

console.log(`Building runtime...`);

await esbuild.build({
  entryPoints: ['src/worker.tsx'],
  bundle: true,
  outfile: join(OUT_DIR, 'extension-worker.js'),
  format: 'cjs',
  platform: 'node',
  external: ["@omnicast/api", "react", "react-reconciler"],
})

const end = performance.now();
const buildTimeMs = end - start;

copyFileSync('./package.json', './dist/package.json');

console.log(`Runtime built in ${Math.round(buildTimeMs)}ms`);

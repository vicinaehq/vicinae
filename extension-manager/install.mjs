import { execSync } from 'child_process';
import { mkdirSync, copyFileSync, rmSync, existsSync, cpSync } from 'fs';
import { homedir } from 'os';
import { join } from 'path';
import { chdir, cwd } from 'process';

const OUT_DIR = "dist";
const BASE_CFG = join(homedir(), ".local", "share", "omnicast", "extensions");

mkdirSync(BASE_CFG, { recursive: true });

const nmPath = join(BASE_CFG, 'node_modules');

if (existsSync(nmPath)) rmSync(nmPath, { recursive: true });

copyFileSync(join(OUT_DIR, "runtime.js"), join(BASE_CFG, "runtime.js"));
copyFileSync(join(OUT_DIR, "package.json"), join(BASE_CFG, "package.json"));

const omniNm = join(nmPath, "@omnicast");

mkdirSync(omniNm, { recursive: true });

const old = cwd();
chdir(BASE_CFG);
execSync("npm install --omit=dev", { stdio: 'inherit' });
chdir(old);

cpSync('../api', join(omniNm, "api"), { recursive: true });

const omniApiNpm = join(omniNm, "api", "node_modules");

if (existsSync(omniApiNpm)) rmSync(omniApiNpm, { recursive: true });

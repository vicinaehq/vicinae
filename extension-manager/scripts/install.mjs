import { mkdirSync, cpSync } from 'fs';
import { homedir } from 'os';
import { join } from 'path';

const OUT_DIR = "dist";
const BASE_CFG = join(homedir(), ".local", "share", "omnicast", "extensions");

mkdirSync(BASE_CFG, { recursive: true });

cpSync(join(OUT_DIR, "runtime.js"), join(BASE_CFG, "manager.js"));

//const runtimeDir = join(BASE_CFG, "runtime");

//mkdirSync(runtimeDir, { recursive: true });

//cpSync(join("runtime", "dist", "extension-worker.js"), join(runtimeDir, "extension-worker.js"));
//cpSync(join("runtime", "package.json"), join(runtimeDir, "package.json"));
//cpSync(join("runtime", "omnicast-api-1.0.0.tgz"), join(runtimeDir, "omnicast-api-1.0.0.tgz"));

import { spawnSync } from "child_process";

const notifyVicinae = () => {
	spawnSync("vicinae", ["vicinae://internal/restart-extension-runtime"]);
};

import "./build.mjs";

notifyVicinae();

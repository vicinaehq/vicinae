#!/usr/bin/env -S node --loader ts-node/esm --disable-warning=ExperimentalWarning

import { main } from "../src/cli.js";

await main();

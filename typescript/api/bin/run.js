#!/usr/bin/env node

const { execute } = require("@oclif/core");

const main = async () => {
  await execute({ dir: __filename });
};

main();

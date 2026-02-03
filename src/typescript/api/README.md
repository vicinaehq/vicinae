This package lets you extend the [Vicinae](https://docs.vicinae.com/) launcher using React and TypeScript.

[![Version](https://img.shields.io/npm/v/@vicinae/api.svg)](https://npmjs.org/package/@vicinae/api)
[![Downloads/week](https://img.shields.io/npm/dw/@vicinae/api.svg)](https://npmjs.org/package/@vicinae/api)

# Getting started

The recommend way to start developing a new extension is to [read the docs](https://docs.vicinae.com/extensions/introduction).

The full API reference (expect breaking changes) can be found [here](https://api-reference.vicinae.com).

# Installation 

Install the package:

```
npm install @vicinae/api
```

# Versioning

The `@vicinae/api` package follows the same versioning as the main `vicinae` binary, since the API is always embedded in the binary.

# CLI usage

The package exports the `vici` binary which is used to build and run extensions in development mode.

While convenience scripts are already provided in the boilerplate, you can still call the binary manually:

```bash
npx vici --help

# assuming vicinae is running
npx vici develop

npx vici build -o my/output/path
```

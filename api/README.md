This package lets you extend the [Vicinae](https://docs.vicinae.com/) launcher using React and TypeScript.

# Getting started

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

# API developers

If you are working on developing the API, it is recommended to point `@vicinae/api` to the local api directory:

```
npm install /path/to/vicinae/source/api
```

Note that you need to recompile the `api` package on every change you make to it.

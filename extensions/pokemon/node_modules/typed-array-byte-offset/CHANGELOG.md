# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [v1.0.3](https://github.com/inspect-js/typed-array-byte-offset/compare/v1.0.2...v1.0.3) - 2024-11-21

### Fixed

- [Fix] avoid relying on `__proto__` accessor [`#4`](https://github.com/inspect-js/typed-array-byte-offset/issues/4)

### Commits

- [types] use shared config [`10b0823`](https://github.com/inspect-js/typed-array-byte-offset/commit/10b0823ecc13b95920cfa8f27fe61af5678fb67b)
- [actions] split out node 10-20, and 20+ [`11554a9`](https://github.com/inspect-js/typed-array-byte-offset/commit/11554a96ca11b85c7ad87118e1d811bfde2b9f32)
- [Dev Deps] update `@arethetypeswrong/cli`, `@ljharb/eslint-config`, `@types/object-inspect`, `auto-changelog`, `object-inspect`, `tape` [`c39dd06`](https://github.com/inspect-js/typed-array-byte-offset/commit/c39dd06d2868a724463722ff2f416b5c41171140)
- [Tests] run tsc and `@arethetypeswrong/cli` in CI [`0b984aa`](https://github.com/inspect-js/typed-array-byte-offset/commit/0b984aa64c86f4bcb476b716cdd16d67c39b68ca)
- [Tests] replace `aud` with `npm audit` [`512b59d`](https://github.com/inspect-js/typed-array-byte-offset/commit/512b59df0e567592282795bfec331193d828f2fc)

## [v1.0.2](https://github.com/inspect-js/typed-array-byte-offset/compare/v1.0.1...v1.0.2) - 2024-02-20

### Commits

- add types [`9eecdd2`](https://github.com/inspect-js/typed-array-byte-offset/commit/9eecdd245b089610d6ad49ef63c9df2b58c3e8a6)
- [actions] skip ls check on node &lt; 10; remove redundant finisher [`4fb4c91`](https://github.com/inspect-js/typed-array-byte-offset/commit/4fb4c912f5eb8034f4e3705b30f3f7dcc7080039)
- [Deps] update `available-typed-arrays`, `has-proto` [`805cee2`](https://github.com/inspect-js/typed-array-byte-offset/commit/805cee207d73e12d526ff23d2c161f38283a1ed9)

## [v1.0.1](https://github.com/inspect-js/typed-array-byte-offset/compare/v1.0.0...v1.0.1) - 2024-02-17

### Commits

- [Dev Deps] update `aud`, `npmignore`, `object-inspect`, `tape` [`ffe7494`](https://github.com/inspect-js/typed-array-byte-offset/commit/ffe7494826fbb6d6bd11c40e03619b12a4ec2266)
- [Deps] update `available-typed-arrays`, `call-bind`, `is-typed-array` [`3006bd7`](https://github.com/inspect-js/typed-array-byte-offset/commit/3006bd7e343d191093802473277801d12bfdc7b2)
- [Refactor] use `gopd` [`45827ea`](https://github.com/inspect-js/typed-array-byte-offset/commit/45827ea7d9709cb1b3a9f2313eed76b71052b9c5)
- [Dev Deps] update `tape` [`e33d080`](https://github.com/inspect-js/typed-array-byte-offset/commit/e33d080ef6488b5f15afe1078a9e5711d9656538)
- [meta] add `sideEffects` flag [`f1dc0db`](https://github.com/inspect-js/typed-array-byte-offset/commit/f1dc0db73c1c4b93c15076602a3e30353878312c)

## v1.0.0 - 2023-06-06

### Commits

- Initial implementation, tests, readme [`f227633`](https://github.com/inspect-js/typed-array-byte-offset/commit/f2276337a907bdfe9725af1b36c3109e76f2430d)
- Initial commit [`806bbaf`](https://github.com/inspect-js/typed-array-byte-offset/commit/806bbaf81e0267aebce5ae68cbf138718513642a)
- npm init [`1151981`](https://github.com/inspect-js/typed-array-byte-offset/commit/1151981427eb1fddab8599d36e6afea50a78293f)
- Only apps should have lockfiles [`5fa9933`](https://github.com/inspect-js/typed-array-byte-offset/commit/5fa9933275f10bdb9e8a175cc70a8228d4811642)

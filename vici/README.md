# @vicinae/vici

Vicinae extension ecosystem CLI

[![Version](https://img.shields.io/npm/v/@vicinae/vici.svg)](https://npmjs.org/package/@vicinae/vici)
[![Downloads/week](https://img.shields.io/npm/dw/@vicinae/vici.svg)](https://npmjs.org/package/@vicinae/vici)

<!-- toc -->
* [@vicinae/vici](#vicinaevici)
* [Usage](#usage)
* [Commands](#commands)
<!-- tocstop -->

# Usage

<!-- usage -->
```sh-session
$ npm install -g @vicinae/vici
$ vici COMMAND
running command...
$ vici (--version)
@vicinae/vici/0.0.0 linux-x64 node-v24.4.0
$ vici --help [COMMAND]
USAGE
  $ vici COMMAND
...
```
<!-- usagestop -->

# Commands

<!-- commands -->
* [`vici build`](#vici-build)
* [`vici develop`](#vici-develop)
* [`vici help [COMMAND]`](#vici-help-command)

## `vici build`

Start an extension development session

```
USAGE
  $ vici build [-o <value>] [-s <value>]

FLAGS
  -o, --out=<value>  Path to output the compiled extension bundle to. Defaults to Vicinae extension directory.
  -s, --src=<value>  [default: /home/aurelle/prog/perso/vicinae/vici] Path to the extension source directory

DESCRIPTION
  Start an extension development session

EXAMPLES
  $ vici build --target /path/to/extension
```

_See code: [src/commands/build/index.ts](https://github.com/vicinaehq/vici/blob/v0.0.0/src/commands/build/index.ts)_

## `vici develop`

Start an extension development session

```
USAGE
  $ vici develop [-i <value>]

FLAGS
  -i, --target=<value>  [default: /home/aurelle/prog/perso/vicinae/vici] Path to the extension directory

DESCRIPTION
  Start an extension development session

EXAMPLES
  $ vici develop --target /path/to/extension
```

_See code: [src/commands/develop/index.ts](https://github.com/vicinaehq/vici/blob/v0.0.0/src/commands/develop/index.ts)_

## `vici help [COMMAND]`

Display help for vici.

```
USAGE
  $ vici help [COMMAND...] [-n]

ARGUMENTS
  COMMAND...  Command to show help for.

FLAGS
  -n, --nested-commands  Include all nested commands in the output.

DESCRIPTION
  Display help for vici.
```

_See code: [@oclif/plugin-help](https://github.com/oclif/plugin-help/blob/v6.2.32/src/commands/help.ts)_
<!-- commandsstop -->

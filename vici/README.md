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
* [`vici develop`](#vici-develop)
* [`vici hello PERSON`](#vici-hello-person)
* [`vici hello world`](#vici-hello-world)
* [`vici help [COMMAND]`](#vici-help-command)

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

## `vici hello PERSON`

Say hello

```
USAGE
  $ vici hello PERSON -f <value>

ARGUMENTS
  PERSON  Person to say hello to

FLAGS
  -f, --from=<value>  (required) Who is saying hello

DESCRIPTION
  Say hello

EXAMPLES
  $ vici hello friend --from oclif
  hello friend from oclif! (./src/commands/hello/index.ts)
```

_See code: [src/commands/hello/index.ts](https://github.com/vicinaehq/vici/blob/v0.0.0/src/commands/hello/index.ts)_

## `vici hello world`

Say hello world

```
USAGE
  $ vici hello world

DESCRIPTION
  Say hello world

EXAMPLES
  $ vici hello world
  hello world! (./src/commands/hello/world.ts)
```

_See code: [src/commands/hello/world.ts](https://github.com/vicinaehq/vici/blob/v0.0.0/src/commands/hello/world.ts)_

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

# @vicinae/vici

Vicinae extension ecosystem CLI

[![Version](https://img.shields.io/npm/v/@vicinae/vici.svg)](https://npmjs.org/package/@vicinae/vici)
[![Downloads/week](https://img.shields.io/npm/dw/@vicinae/vici.svg)](https://npmjs.org/package/@vicinae/vici)

<!-- toc -->

- [Usage](#usage)
- [Commands](#commands)
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

- [`vici hello PERSON`](#vici-hello-person)
- [`vici hello world`](#vici-hello-world)
- [`vici help [COMMAND]`](#vici-help-command)
- [`vici plugins`](#vici-plugins)
- [`vici plugins add PLUGIN`](#vici-plugins-add-plugin)
- [`vici plugins:inspect PLUGIN...`](#vici-pluginsinspect-plugin)
- [`vici plugins install PLUGIN`](#vici-plugins-install-plugin)
- [`vici plugins link PATH`](#vici-plugins-link-path)
- [`vici plugins remove [PLUGIN]`](#vici-plugins-remove-plugin)
- [`vici plugins reset`](#vici-plugins-reset)
- [`vici plugins uninstall [PLUGIN]`](#vici-plugins-uninstall-plugin)
- [`vici plugins unlink [PLUGIN]`](#vici-plugins-unlink-plugin)
- [`vici plugins update`](#vici-plugins-update)

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

## `vici plugins`

List installed plugins.

```
USAGE
  $ vici plugins [--json] [--core]

FLAGS
  --core  Show core plugins.

GLOBAL FLAGS
  --json  Format output as json.

DESCRIPTION
  List installed plugins.

EXAMPLES
  $ vici plugins
```

_See code: [@oclif/plugin-plugins](https://github.com/oclif/plugin-plugins/blob/v5.4.46/src/commands/plugins/index.ts)_

## `vici plugins add PLUGIN`

Installs a plugin into vici.

```
USAGE
  $ vici plugins add PLUGIN... [--json] [-f] [-h] [-s | -v]

ARGUMENTS
  PLUGIN...  Plugin to install.

FLAGS
  -f, --force    Force npm to fetch remote resources even if a local copy exists on disk.
  -h, --help     Show CLI help.
  -s, --silent   Silences npm output.
  -v, --verbose  Show verbose npm output.

GLOBAL FLAGS
  --json  Format output as json.

DESCRIPTION
  Installs a plugin into vici.

  Uses npm to install plugins.

  Installation of a user-installed plugin will override a core plugin.

  Use the VICI_NPM_LOG_LEVEL environment variable to set the npm loglevel.
  Use the VICI_NPM_REGISTRY environment variable to set the npm registry.

ALIASES
  $ vici plugins add

EXAMPLES
  Install a plugin from npm registry.

    $ vici plugins add myplugin

  Install a plugin from a github url.

    $ vici plugins add https://github.com/someuser/someplugin

  Install a plugin from a github slug.

    $ vici plugins add someuser/someplugin
```

## `vici plugins:inspect PLUGIN...`

Displays installation properties of a plugin.

```
USAGE
  $ vici plugins inspect PLUGIN...

ARGUMENTS
  PLUGIN...  [default: .] Plugin to inspect.

FLAGS
  -h, --help     Show CLI help.
  -v, --verbose

GLOBAL FLAGS
  --json  Format output as json.

DESCRIPTION
  Displays installation properties of a plugin.

EXAMPLES
  $ vici plugins inspect myplugin
```

_See code: [@oclif/plugin-plugins](https://github.com/oclif/plugin-plugins/blob/v5.4.46/src/commands/plugins/inspect.ts)_

## `vici plugins install PLUGIN`

Installs a plugin into vici.

```
USAGE
  $ vici plugins install PLUGIN... [--json] [-f] [-h] [-s | -v]

ARGUMENTS
  PLUGIN...  Plugin to install.

FLAGS
  -f, --force    Force npm to fetch remote resources even if a local copy exists on disk.
  -h, --help     Show CLI help.
  -s, --silent   Silences npm output.
  -v, --verbose  Show verbose npm output.

GLOBAL FLAGS
  --json  Format output as json.

DESCRIPTION
  Installs a plugin into vici.

  Uses npm to install plugins.

  Installation of a user-installed plugin will override a core plugin.

  Use the VICI_NPM_LOG_LEVEL environment variable to set the npm loglevel.
  Use the VICI_NPM_REGISTRY environment variable to set the npm registry.

ALIASES
  $ vici plugins add

EXAMPLES
  Install a plugin from npm registry.

    $ vici plugins install myplugin

  Install a plugin from a github url.

    $ vici plugins install https://github.com/someuser/someplugin

  Install a plugin from a github slug.

    $ vici plugins install someuser/someplugin
```

_See code: [@oclif/plugin-plugins](https://github.com/oclif/plugin-plugins/blob/v5.4.46/src/commands/plugins/install.ts)_

## `vici plugins link PATH`

Links a plugin into the CLI for development.

```
USAGE
  $ vici plugins link PATH [-h] [--install] [-v]

ARGUMENTS
  PATH  [default: .] path to plugin

FLAGS
  -h, --help          Show CLI help.
  -v, --verbose
      --[no-]install  Install dependencies after linking the plugin.

DESCRIPTION
  Links a plugin into the CLI for development.

  Installation of a linked plugin will override a user-installed or core plugin.

  e.g. If you have a user-installed or core plugin that has a 'hello' command, installing a linked plugin with a 'hello'
  command will override the user-installed or core plugin implementation. This is useful for development work.


EXAMPLES
  $ vici plugins link myplugin
```

_See code: [@oclif/plugin-plugins](https://github.com/oclif/plugin-plugins/blob/v5.4.46/src/commands/plugins/link.ts)_

## `vici plugins remove [PLUGIN]`

Removes a plugin from the CLI.

```
USAGE
  $ vici plugins remove [PLUGIN...] [-h] [-v]

ARGUMENTS
  PLUGIN...  plugin to uninstall

FLAGS
  -h, --help     Show CLI help.
  -v, --verbose

DESCRIPTION
  Removes a plugin from the CLI.

ALIASES
  $ vici plugins unlink
  $ vici plugins remove

EXAMPLES
  $ vici plugins remove myplugin
```

## `vici plugins reset`

Remove all user-installed and linked plugins.

```
USAGE
  $ vici plugins reset [--hard] [--reinstall]

FLAGS
  --hard       Delete node_modules and package manager related files in addition to uninstalling plugins.
  --reinstall  Reinstall all plugins after uninstalling.
```

_See code: [@oclif/plugin-plugins](https://github.com/oclif/plugin-plugins/blob/v5.4.46/src/commands/plugins/reset.ts)_

## `vici plugins uninstall [PLUGIN]`

Removes a plugin from the CLI.

```
USAGE
  $ vici plugins uninstall [PLUGIN...] [-h] [-v]

ARGUMENTS
  PLUGIN...  plugin to uninstall

FLAGS
  -h, --help     Show CLI help.
  -v, --verbose

DESCRIPTION
  Removes a plugin from the CLI.

ALIASES
  $ vici plugins unlink
  $ vici plugins remove

EXAMPLES
  $ vici plugins uninstall myplugin
```

_See code: [@oclif/plugin-plugins](https://github.com/oclif/plugin-plugins/blob/v5.4.46/src/commands/plugins/uninstall.ts)_

## `vici plugins unlink [PLUGIN]`

Removes a plugin from the CLI.

```
USAGE
  $ vici plugins unlink [PLUGIN...] [-h] [-v]

ARGUMENTS
  PLUGIN...  plugin to uninstall

FLAGS
  -h, --help     Show CLI help.
  -v, --verbose

DESCRIPTION
  Removes a plugin from the CLI.

ALIASES
  $ vici plugins unlink
  $ vici plugins remove

EXAMPLES
  $ vici plugins unlink myplugin
```

## `vici plugins update`

Update installed plugins.

```
USAGE
  $ vici plugins update [-h] [-v]

FLAGS
  -h, --help     Show CLI help.
  -v, --verbose

DESCRIPTION
  Update installed plugins.
```

_See code: [@oclif/plugin-plugins](https://github.com/oclif/plugin-plugins/blob/v5.4.46/src/commands/plugins/update.ts)_

<!-- commandsstop -->

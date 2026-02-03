The extension manager is a node process that can load and unload extensions at will.
It is spawned by the `vicinae server` command.

Communication is done through standard file descriptors:

- Standard output is used for the message scheme (protobuf)
- Standard error is used for debug messages

Note that since standard output is already assigned to the message scheme you should NOT use `console.log` but
`console.error` during development; otherwise you will end up interferring with the communication.

## About versioning

The `version` field of the `package.json` file is not meaningful. The manager is bundled with every vicinae release and thus shares the same version as the release it's built in.

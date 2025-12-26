This document contains a set of guidelines you need to follow before contributing to vicinae, be it through a bug report or code.

## Raising issues

All issues are tracked using the built-in GitHub issue tracker. If you are not sure your problem justifies creating a new issue, you can create a GitHub discussion or join the Discord server.

Before opening an issue, do a quick search to make sure you are not creating a duplicate.

Make sure to follow the issue template that is provided. In particular, bug issues should be submitted using the "Report Bug" command from vicinae directly, unless the bug results in the vicinae server not being able to start.

If you want to report a bug relative to an extension published in the official vicinae store, you should open the issue in the [extension repository](https://github.com/vicinaehq/extensions), not in the main vicinae repository.

If you think you've found a severe security issue, you should contact the email address listed as the contact address for the [vicinaehq organization](https://github.com/vicinaehq).

## Contributing code

### General guidelines

Less is more: each new line of code represents additional maintenance for the project and huge PRs have lower odds of being accepted, especially if they involve significant architectural commitments that were not discussed before.

If you want to work on a big change, you should probably join the [discord server](https://vicinae.com/discord) and discuss it in the dev channel beforehand.

All submitted code needs to be locally tested. See [this page of the documentation](https://docs.vicinae.com/build) for build instructions and development tips.

### Formatting and linting

Formatting is done with `clang-format`, as prescribed by the `.clang-format` file present at the root of the repository. Contributions must respect the format. You can run `make format` to apply formatting to all project files at once, but your IDE should be able to pick up on it and automatically format the code.

Keep the number of comments to a strict minimum, good code shouldn't need many comments. There are good use cases for comments though: if you feel like your solution to a given problem is not ideal, could be improved, or relies on a weird hack, using a comment to document it is encouraged. We don't use documentation generators at the moment, so such type of comments are not needed.

A `.clang-tidy` configuration file was added very recently to the project, but it's not currently enforced because a lot of code has yet to be migrated. New code **must** be checked against these rules. Most IDEs should be able to automatically provide intellisense based on the presence of the `.clang-tidy` file alone.

### AI generated code

AI generated code is treated the same as regular code. As such, all the aforementioned rules apply.

AI is **not** a substitute for properly understanding and testing your code: don't be lazy. Lazy AI PRs that do not respect the guidelines will be rejected. In particular, keep your pull request's description as concise as possible: no maintainer will read your novel.

If your contribution was mostly AI generated, it's considered good practice to indicate what model or tool you used for that.


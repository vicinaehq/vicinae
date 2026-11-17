# Nix

## Bundled input server with Home Manager on NixOS

The input server lets Vicinae simulate keyboard input, including automatically
pasting dictation results. Home Manager can select the server bundled with your
Vicinae package, but it cannot grant access to the system's input devices.

### 1. Grant uinput access once

Add the following to your **NixOS system configuration**, replacing `yourUsername`
with your login name:

```nix
{
  hardware.uinput.enable = true;
  users.users.yourUsername.extraGroups = [ "uinput" ];
}
```

Apply your NixOS configuration, then fully log out and back in so the new group
membership takes effect. If these permissions are already configured, skip this
step.

**Security:** membership in the `uinput` group lets any process running as your
user simulate keyboard input. It does not grant permission to monitor physical
keyboards. Features that require input monitoring, such as automatic snippet
expansion, need additional permissions that this setup does not grant.

### 2. Enable the bundled server in Home Manager

With the Vicinae flake available as `inputs.vicinae`, add the following to your
**Home Manager configuration** (pass `inputs` through Home Manager's
`extraSpecialArgs` if it is not already available):

```nix
{ inputs, ... }: {
  imports = [ inputs.vicinae.homeManagerModules.default ];

  programs.vicinae = {
    enable = true;
    inputServer.enable = true;
  };
}
```

Apply your Home Manager configuration and restart Vicinae.

This uses the input server from the same package as Vicinae. No package override,
manual `VICINAE_INPUT_SERVER_BIN` setting, or Vicinae NixOS module is needed.
Remove an existing manual `VICINAE_INPUT_SERVER_BIN` setting unless you want to
keep overriding the selected server. Custom packages must honor that variable.

If you already use the Vicinae NixOS module, leave
`programs.vicinae.inputServer.enable` unset in Home Manager to keep using the
privileged input server. The uinput setup above is only needed when choosing the
unprivileged bundled server instead.

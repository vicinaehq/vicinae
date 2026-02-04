<p align="center">
  <img
    width="100"
    src="extra/vicinae.png"
    alt="Vicinae text logo"
  />
</p>

[![Discord](https://discord.com/api/guilds/1407488619765174273/widget.png?style=shield&nonce=1756955967)](https://discord.gg/rP4ecD42p7)

**Vicinae** (pronounced _"vee-CHEE-nay"_) is a high-performance, native launcher for your desktop.

It comes with a rich set of built-in modules and can be easily extended using the [Typescript SDK](https://docs.vicinae.com/extensions/introduction).

Drawing inspiration from the [Raycast launcher](https://www.raycast.com/), Vicinae provides a mostly compatible extension API, allowing reuse of many existing Raycast extensions with minimal modification.

Vicinae is designed for developers and power users who want fast, keyboard-first access to common system actions.

Check the [website](https://vicinae.com) ;)

<p align="center">
  <video alt="demo video" src="https://private-user-images.githubusercontent.com/72200344/499625615-b5831ba7-dd16-407e-a05c-c2dbb468a0d6.mp4?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NzAxNDI4NjEsIm5iZiI6MTc3MDE0MjU2MSwicGF0aCI6Ii83MjIwMDM0NC80OTk2MjU2MTUtYjU4MzFiYTctZGQxNi00MDdlLWEwNWMtYzJkYmI0NjhhMGQ2Lm1wND9YLUFtei1BbGdvcml0aG09QVdTNC1ITUFDLVNIQTI1NiZYLUFtei1DcmVkZW50aWFsPUFLSUFWQ09EWUxTQTUzUFFLNFpBJTJGMjAyNjAyMDMlMkZ1cy1lYXN0LTElMkZzMyUyRmF3czRfcmVxdWVzdCZYLUFtei1EYXRlPTIwMjYwMjAzVDE4MTYwMVomWC1BbXotRXhwaXJlcz0zMDAmWC1BbXotU2lnbmF0dXJlPWFhYjI5ZTJiMDkyYjliMDFjYWYzZDkxMmI5YWRhYmFkYzI0MzkzNjFmNjQzMDRmNjc2ZGRiMGVlNTlkOTViNTUmWC1BbXotU2lnbmVkSGVhZGVycz1ob3N0In0.ksY1Ik5XC5xCp1K7rTg2XRlL_8-qk9E2WqzkeLjHrMo" />
</p>

## Features

- Launch apps
- File search
- Emoji picker 
- Calculator with history (multiple backends available)
- Clipboard history
- Quickly open links using any app (can be used to set up web search)
- Focus open windows
- script commands
- `dmenu` compatibility mode
- Theming system 
- Fallback commands
- React/Typescript extension SDK, allowing to build complex search-focused UIs
- Global [extension store](https://github.com/vicinaehq/extensions) (since v0.16.0)
- Raycast compatibility support  
  → Includes access to the official Raycast extension store, with one-click installs directly from within the launcher  
  → Many extensions may not work yet due to missing APIs or general Linux incompatibilities


## Breaking changes

Until we reach version 1 you should expect breaking changes from release to release. Watch the changelogs to learn about them.

We try to not introduce breaking changes that result in a definitive loss of data, but you should expect to have to redo your configuration from time to time.

## Documentation

All you need to know is at [docs.vicinae.com](https://docs.vicinae.com).

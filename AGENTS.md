vicinae is a multi purpose launcher built with QtQuick. This file focuses on explaining general rules. For everything else, just look at the codebase.

It uses QML for presentation and C++ for business logic.
React/Typescript is used to power the extension API.

Build debug: `make debug`.

## Separation of concerns

QML is exclusively used for presentation. DO NOT put business logic inside QML files.
It's okay to make small JavaScript functions and utilities as long as their purpose is to help deal with presentation concerns.

To implement views, we use a bridge pattern: a C++ "view host" class exposes data and actions as `Q_PROPERTY`s and `Q_INVOKABLE`s, and a QML component binds to them.

Remain pragmatic about this, we don't want to go back and forth between QML and C++ to execute 2 lines of logic. But most of the time, the separation is clear.

## C++/QT coding rules

We use C++23 so we have access to most modern C++ features. 
Here are a few rules to keep in mind:

- Lack of value: use `std::optional` instead of arbitrary value discriminants such as the empty string. If this is not possible or goes against a commonly used convention, respect the convention first, no shoehorning.
If we are dealing with raw pointers, the nullable component is already part of it so no need to add a layer of indirection.
- Avoid raw pointers: unless we are dealing with QT's ownership model. For QT classes that are not QObjects, you should probably use standard smart pointers as recommended in modern QT.
- Watch for implicit copies: avoid copies as much as possible, use non owning containers when you can (`std::span`, `std::string_view`, `QStringView`) and just `std::move` the data when applicable. When copy is the safest option, use copy: don't go out of your way to respect this rule.
- QT vs STL classes: prefer STL containers over QT counterparts.
- vectors: always reserve `std::vector`s and use `emplace_back` to push new elements.
- STL ranges: we like to use `<ranges>` where it works well. If you need to use a `for` loop, prioritize ranged ones or use `std::views::enumerate` if you need to deal with indexes and data.

## Coding style

- For QObject classes: always put `Q_OBJECT`, `Q_PROPERTY`, `Q_INVOKABLE`, and `signals` on top of the class, in this order.
- Do not add unnecessary comments. Use them sparingly when something really needs to be made explicit.
- Include order: system headers before local headers.
- Header guards: use `#pragma once`.
- String types: use `std::string` internally, `QString` only at the Qt boundary.

## QML rules

When writing JavaScript inside QML files, use ES6 syntax to the largest extent possible.

Try to keep the amount of logic in these files small. Logic in QML is only for presentation concerns: metrics computation, hover on signal, etc...

Some configuration and theming options may need to be accessed directly in QML. We expose a global config and theme bridges for this use case.

## React/TypeScript extensions

Everything related to the Typescript SDK can be found under `src/typescript`. More information can be obtained by reading `README.md` files under this directory if you need to work on this part. 

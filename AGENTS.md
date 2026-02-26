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
- Avoidpreferrableraw pointers: unless we are dealing with QT's ownership model. For QT classes that are not QObjects, you should probably use standard smart pointers as recommended in modern QT.
- Watch for implicit copies: avoid copies as much as possible, use non owning containers when you can (`std::span`, `std::string_view`, `QStringView`) and just `std::move` the data when applicable. When copy is the safest option, use copy: don't go out of your way to respect this rule.
- QT vs STL classes: prefer STL containers over QT counterparts.
- vectors: always reserve `std::vector`s and use `emplace_back` to push new elements.
- STL ranges: we like to use `<ranges>` where it works well. If you need to use a `for` loop, prioritize ranged ones or use `std::views::enumerate` if you need to deal with indexes and data.
- casts: as you may notice from the clang-tidy rules, using `static_cast` is generally discouraged for classes that are related by inheritance, as it is very easy to make mistakes. Unless the logic is in a hot path (e.g a QT event filter), it's preferrable to use `dynamic_cast` as a safety net when there is no safer option. Note however that `dynamic_cast` is STILL a code smell and should generally be avoided in favor of better polymorphism.

## Coding style

- For QObject classes: always put `Q_OBJECT`, `Q_PROPERTY`, `Q_INVOKABLE`, and `signals` on top of the class, in this order.
- Do not add unnecessary comments. Use them sparingly when something really needs to be made explicit.
- Include order: system headers before local headers.
- Header guards: use `#pragma once`.
- String types: use `std::string` internally, `QString` only at the Qt boundary.
- Do not overuse comments, only add them when they provide actual value

## Linting and formatting

We format all our code using `clang-format`. We have a `make format` rule that will automatically format the entire codebase if necessary.

All our code is also linted with `clang-tidy` in order to make detecting common mistakes easier. `clang-tidy` violations may be acceptable under some circumstances, and should be implemented using `//NOLINTBEGIN(<rule>)` and `NOLINTEND(<rule>)`. comments. Inline `//NOLINT` comments are generally discouraged because they can break after formatting. If a nolint directive does not work (e.g we're dealing with an internal STL false positive) then you can do a local override of the `clang-tidy` configuration to explicitly disable the faulty check.

## QML rules

When writing JavaScript inside QML files, use ES6 syntax to the largest extent possible.

Try to keep the amount of logic in these files small. Logic in QML is only for presentation concerns: metrics computation, hover on signal, etc...

Some configuration and theming options may need to be accessed directly in QML. We expose a global config and theme bridges for this use case.

## React/TypeScript extensions

Everything related to the Typescript SDK can be found under `src/typescript`. More information can be obtained by reading `README.md` files under this directory if you need to work on this part. 

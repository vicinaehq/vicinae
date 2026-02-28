#pragma once
#include "qml/bridge-view.hpp"
#include "single-view-command-context.hpp"
#include "vicinae.hpp"

class MarkdownShowcaseView : public FormViewBase {
  Q_OBJECT

  Q_PROPERTY(QString introMarkdown READ introMarkdown CONSTANT)

public:
  QUrl qmlComponentUrl() const override { return QUrl(QStringLiteral("qrc:/Vicinae/StoreIntroView.qml")); }

  QVariantMap qmlProperties() override { return {{QStringLiteral("host"), QVariant::fromValue(this)}}; }

  QString introMarkdown() const {
    return QStringLiteral(R"(# Heading 1

## Heading 2

### Heading 3

#### Heading 4

##### Heading 5

This is a regular paragraph with **bold text**, *italic text*, and ***bold italic***. Here is some `inline code` and a [link to Vicinae](https://vicinae.com). You can also use ~~strikethrough~~ text.

Here is a paragraph with a soft
line break (soft break) and a hard line break below:

Line before hard break\
Line after hard break

---

## Code Blocks

```cpp
#include <iostream>

int main() {
    std::cout << "Hello from C++!" << std::endl;
    return 0;
}
```

```python
def fibonacci(n):
    a, b = 0, 1
    for _ in range(n):
        yield a
        a, b = b, a + b

for num in fibonacci(10):
    print(num)
```

```javascript
async function fetchUsers(endpoint) {
  const response = await fetch(endpoint);
  if (!response.ok) throw new Error(`HTTP ${response.status}`);
  const { data, meta } = await response.json();
  return data.filter(user => user.active).map(({ id, name }) => ({ id, name }));
}
```

```rust
fn main() {
    let numbers: Vec<i32> = (1..=10).collect();
    let sum: i32 = numbers.iter()
        .filter(|&&x| x % 2 == 0)
        .sum();
    println!("Sum of evens: {sum}");
}
```

```bash
#!/bin/bash
for file in *.log; do
    count=$(grep -c "ERROR" "$file" 2>/dev/null)
    [[ $count -gt 0 ]] && echo "$file: $count errors"
done
```

```json
{
  "name": "vicinae",
  "version": "1.0.0",
  "features": ["search", "clipboard", "extensions"],
  "config": { "theme": "dark", "fontSize": 14, "enabled": true }
}
```

```
Plain code block without a language tag.
Just some preformatted text.
```

## Lists

### Unordered List

- First item
- Second item with **bold**
- Third item with `inline code`
  - Nested item A
  - Nested item B
    - Deeply nested item

### Ordered List

1. First step
2. Second step
3. Third step
   1. Sub-step A
   2. Sub-step B

## Tables

| Feature | Status | Notes |
|---------|:------:|------:|
| Headings | Done | H1 through H5 |
| **Bold** text | Done | In paragraphs, lists, tables |
| Code blocks | Done | With language labels & copy |
| Tables | Done | With column alignment |
| Images | Done | HTTP, local, data URI, builtin |

## Blockquotes

> This is a plain blockquote. It uses a left border and muted, italic text.

> This is a multi-paragraph blockquote.
>
> It has a second paragraph with **bold** and *italic* text.

## Callouts

> [!NOTE]
> This is a note callout. Use it for helpful information that users should be aware of.

> [!TIP]
> This is a tip. Use it for helpful advice or best practices.

> [!IMPORTANT]
> This is an important callout. Use it for crucial information.

> [!WARNING]
> This is a warning. Use it to alert users about potential issues.

> [!CAUTION]
> This is a caution. Use it for actions that could cause problems.

## Images

![Vicinae icon](image://vicinae/builtin:cart?raycast-width=32&raycast-height=32)

[![Pikachu](https://archives.bulbagarden.net/media/upload/thumb/4/4a/0025Pikachu.png/600px-0025Pikachu.png)](https://bulbapedia.bulbagarden.net/wiki/Pikachu)

![HTTP GIF](https://docs.vicinae.com/scripts/full-output-demo.gif)

---

*End of markdown showcase.*
)");
  }
};

class MarkdownShowcase : public BuiltinViewCommand<MarkdownShowcaseView> {
  QString id() const override { return "markdown-showcase"; }
  QString name() const override { return "Markdown Showcase"; }
  QString description() const override { return "Preview all supported markdown features"; }
  bool isFallback() const override { return false; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("text").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  bool isInternal() const override { return true; }
};

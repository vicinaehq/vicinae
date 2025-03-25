#include "app.hpp"
#include "ui/markdown/markdown-renderer.hpp"
#include "view.hpp"

static const QString text = R"(
# The Art of Simplicity in Programming

## Introduction

In the world of software development, complexity often creeps in unnoticed. As codebases grow, they tend to become more intricate, harder to maintain, and increasingly difficult to understand. This essay explores the **profound value of simplicity** in programming and why it should be a guiding principle for developers at all levels.

## The Problem with Complexity

When we write code, we often fall into the trap of premature optimization or over-engineering. We create elaborate architectures to solve problems we *might* face in the future. As the famous quote by Donald Knuth suggests: `premature optimization is the root of all evil`.

Complex code has several disadvantages:

- It's harder to understand for new team members
- It's more prone to bugs and edge cases
- It requires more extensive documentation
- It takes longer to modify when requirements change

## Embracing Simplicity

Simplicity doesn't mean being simplistic. Rather, it means finding the **most straightforward solution** that fully addresses the current problem. As Leonardo da Vinci once said, "*Simplicity is the ultimate sophistication*."

When writing code, consider these principles:

1. **Write for humans first**, computers second
2. *Use clear naming* conventions for variables and functions
3. Break complex functions into **_smaller, focused ones_**
4. Avoid premature abstraction with `if (isNeeded) { createAbstraction() }`

## The Long-Term Benefits

Code that embraces simplicity pays dividends over time. It's more **maintainable**, *easier to test*, and generally more robust. New team members can onboard faster, and the codebase remains adaptable to changing requirements.

Test the test the test.

## Conclusion

In our pursuit of elegant solutions, we should remember that the most elegant code is often the simplest. As you develop your next project, ask yourself: "*Is there a simpler way to solve this problem?*" The answer might lead you to better, more maintainable code.

Remember what Antoine de Saint-Exupéry wisely noted: "*Perfection is achieved not when there is nothing more to add, but when there is nothing left to take away.*"
)";

class TestView : public View {
  MarkdownRenderer *_renderer;
  QList<QString> tokens = text.split(" ");
  QTimer *timer = new QTimer(this);
  size_t i = 0;

public:
  TestView(AppWindow &app) : View(app), _renderer(new MarkdownRenderer) {

    //_renderer->setMarkdown("# Test\n\n");
    //_renderer->setMarkdown(text);

    connect(timer, &QTimer::timeout, this, [this]() {
      qDebug() << i << "/" << tokens.size();
      if (i >= tokens.size()) {
        timer->stop();
      } else {
        if (i > 0) _renderer->appendMarkdown(" ");
        _renderer->appendMarkdown(tokens[i++]);
      }
    });

    timer->start(20);

    widget = _renderer;
  }
};

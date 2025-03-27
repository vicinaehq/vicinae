#include "app.hpp"
#include "ui/markdown/markdown-renderer.hpp"
#include "view.hpp"

const static QString text2 =
    "Certainly! Here's a list of arguments that explain why clean code is great:\n\n1. **Improved "
    "Maintainability:**\n   - Clean code is easier to maintain and update over time, reducing the effort "
    "required for bug fixes and feature enhancements.\n\n2. **Enhanced Readability:**\n   - Well-structured "
    "and well-commented code is more readable, making it easier for other developers (and sometimes even "
    "yourself) to understand the logic and flow of the program.\n\n3. **Increased Collaboration:**\n   - "
    "Clean code facilitates better collaboration among team members. When code is easy to read and follow, "
    "onboarding new developers becomes more straightforward.\n\n4. **Reduced Technical Debt:**\n   - Writing "
    "clean code from the start helps avoid accumulating technical debt, which can slow down development and "
    "increase long-term costs.\n\n5. **Faster Debugging:**\n   - Clean and organized code makes it easier to "
    "identify and fix bugs, reducing the time spent on debugging.\n\n6. **Consistency in Code Style:**\n   - "
    "Following clean code practices ensures consistency across the project or organization, making it more "
    "predictable and easier for developers to adapt.\n\n7. **Better Documentation:**\n   - Clean code often "
    "includes meaningful variable names, clear functions, and concise comments that serve as implicit "
    "documentation, reducing the need for separate documents.\n\n8. **Higher Developer Productivity:**\n   - "
    "Developers can work more efficiently with clean code, as they spend less time trying to understand "
    "complex or poorly written code.\n\n9. **Improved Code Reusability:**\n   - Clean code is modular and "
    "well-structured, making it easier to reuse components across different projects or within the same "
    "project.\n\n10. **Easier Testing:**\n    - Clean code with clear functions and methods is more "
    "testable. Unit tests can be written more easily for well-defined pieces of logic.\n\n11. **Reduced "
    "Cognitive Load:**\n    - Clean code reduces the cognitive load on developers, allowing them to focus on "
    "solving problems rather than deciphering complex or convoluted code.\n\n12. **Faster Onboarding:**\n    "
    "- New team members can get up to speed more quickly with clean code, as it is easier to read and "
    "understand, reducing the learning curve.\n\n13. **Higher Code Quality:**\n    - Clean code promotes "
    "better practices such as refactoring, which leads to higher overall code quality and "
    "maintainability.\n\n14. **Increased Developer Satisfaction:**\n    - Developers tend to feel more "
    "satisfied and proud of their work when they write clean code, leading to a positive work "
    "environment.\n\n15. **Long-Term Cost Savings:**\n    - While it might take more time upfront to write "
    "clean code, the long-term benefits in terms of reduced maintenance costs, faster development cycles, "
    "and higher code quality make it a worthwhile investment.\n\n16. **Easier Refactoring:**\n    - Clean "
    "code is easier to refactor, as its structure and organization make changes less risky and more "
    "predictable.\n\n17. **Better Compliance with Standards:**\n    - Following clean code practices helps "
    "ensure that the code complies with industry standards and best practices.\n\n18. **Improved Code "
    "Reviews:**\n    - Clean code makes code reviews more efficient, as reviewers can quickly understand "
    "what changes are being made and why.\n\nThese arguments highlight the numerous benefits of writing "
    "clean code, making it a valuable practice for any software development project.";

const QString text = R"(
Here's a markdown list:

- Item 1
- Item 2
- Item 3
- Item 4
- Item 5

And now some code:
```c
int main() {
	printf("Hello, world!\n");
}
```

And some paragraph.

Some `other` paragraph right here. Enjoy please.
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
    //

    _renderer->setMarkdown(text);

    /*
connect(timer, &QTimer::timeout, this, [this]() {
  qDebug() << i << "/" << tokens.size();
  if (i >= tokens.size()) {
    timer->stop();
  } else {
    QString s = i > 0 ? " " : "";

    s += tokens[i++];
    _renderer->appendMarkdown(s);
  }
});

timer->start(10);
    */

    widget = _renderer;
  }
};

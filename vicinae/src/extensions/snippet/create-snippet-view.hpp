#pragma once
#include "services/snippet/snippet-db.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/checkbox-input.hpp"
#include "ui/form/text-area.hpp"
#include "ui/views/form-view.hpp"
#include "service-registry.hpp"
#include "services/snippet/snippet-service.hpp"
#include "services/toast/toast-service.hpp"
#include "utils.hpp"

class BasicFormSnippetView : public ManagedFormView {
public:
  BasicFormSnippetView() {
    m_name->setPlaceholderText("Euro symbol");
    m_content->setPlaceholderText("€");
    m_keyword->setPlaceholderText(":!euro");
    m_word->setValueAsJson(true);
  }

  virtual void initializeSnippetForm() {}

  void initializeForm() final override {
    m_service = context()->services->snippetService();
    auto nameField = form()->addField();
    nameField->setName("Title");
    nameField->setWidget(m_name);

    auto contentField = form()->addField();
    contentField->setName("Content");
    contentField->setInfo("You can enrich the content with {dynamic placeholder}");
    contentField->setWidget(m_content);

    form()->addSeparator();

    auto keywordField = form()->addField();
    keywordField->setName("Keyword");
    keywordField->setInfo(
        "When typing this text anywhere, it will be automatically expanded to this snippet's content.");
    keywordField->setWidget(m_keyword);

    auto wordCheckbox = form()->addField();
    wordCheckbox->setName("Expand as word");
    wordCheckbox->setWidget(m_word);
    wordCheckbox->setInfo(
        "If a keyword is typed, it will only be expanded after space or return is pressed.");

    auto init = initialData();

    setSnippetData(init);

    if (!init.name.empty()) { m_name->selectAll(); }

    initializeSnippetForm();
  }

  virtual SnippetDatabase::SnippetPayload initialData() { return {}; }

  virtual bool handleSubmit(SnippetDatabase::SnippetPayload payload) = 0;

  void setSnippetData(SnippetDatabase::SnippetPayload snippet) {
    m_name->setText(snippet.name.c_str());

    if (const auto exp = snippet.expansion) {
      m_keyword->setText(exp->keyword.c_str());
      m_word->setValueAsJson(exp->word);
    }

    const auto visitor =
        overloads{[&](const SnippetDatabase::TextSnippet &text) { m_content->setText(text.text.c_str()); },
                  [&](const SnippetDatabase::FileSnippet &file) { m_content->setText(file.file.c_str()); }};

    std::visit(visitor, snippet.data);
  }

  void onSubmit() override {
    const auto toast = context()->services->toastService();
    const auto name = m_name->text();
    const auto slug = slugify(name);
    const auto content = m_content->text();
    const auto keyword = m_keyword->text();

    form()->clearAllErrors();

    if (name.size() < 2) { form()->setError(m_name, "2 chars min."); }
    if (m_content->text().isEmpty()) { form()->setError(m_content, "Content should not be empty"); }

    if (!keyword.isEmpty()) {
      const bool hasSpaces = std::ranges::any_of(keyword, [](QChar c) { return c.isSpace(); });
      if (hasSpaces) { form()->setError(m_keyword, "No spaces"); }
    }

    if (!form()->isValid()) {
      toast->failure("Validation failed");
      return;
    }

    SnippetDatabase::SnippetPayload payload;
    payload.name = name.toStdString();
    payload.data = SnippetDatabase::TextSnippet(content.toStdString());

    if (!keyword.isEmpty()) {
      SnippetDatabase::Expansion expansion;
      expansion.keyword = keyword.toStdString();
      expansion.word = m_word->value();
      payload.expansion = expansion;
    }

    if (handleSubmit(payload)) { popSelf(); }
  }

protected:
  SnippetService *m_service = nullptr;
  BaseInput *m_name = new BaseInput;
  TextArea *m_content = new TextArea;
  BaseInput *m_keyword = new BaseInput;
  CheckboxInput *m_word = new CheckboxInput;
};

class CreateSnippetView : public BasicFormSnippetView {
public:
  bool handleSubmit(SnippetDatabase::SnippetPayload payload) override {
    const auto toast = context()->services->toastService();
    const auto result = m_service->createSnippet(payload);

    if (!result) {
      toast->failure(result.error().c_str());
      return false;
    }

    toast->success("Snippet successfully created");
    return true;
  }
};

class EditSnippetView : public BasicFormSnippetView {
public:
  EditSnippetView(SnippetDatabase::SerializedSnippet snippet) : m_snippet(snippet) {}

  SnippetDatabase::SnippetPayload initialData() override {
    return SnippetDatabase::SnippetPayload{
        .name = m_snippet.name,
        .data = m_snippet.data,
        .expansion = m_snippet.expansion,
    };
  }

  void initializeSnippetForm() override { setNavigationTitle(QString("Edit \"%1\"").arg(m_snippet.name)); }

  bool handleSubmit(SnippetDatabase::SnippetPayload payload) override {
    const auto toast = context()->services->toastService();
    const auto result = m_service->updateSnippet(m_snippet.id, payload);

    if (!result) {
      toast->failure(result.error().c_str());
      return false;
    }

    toast->success("Snippet updated");
    return true;
  }

private:
  SnippetDatabase::SerializedSnippet m_snippet;
};

class DuplicateSnippetView : public CreateSnippetView {
public:
  DuplicateSnippetView(SnippetDatabase::SerializedSnippet snippet) : m_snippet(snippet) {}

  SnippetDatabase::SnippetPayload initialData() override {
    return SnippetDatabase::SnippetPayload{
        .name = std::format("Copy of {}", m_snippet.name),
        .data = m_snippet.data,
    };
  }

  void initializeSnippetForm() override {
    setNavigationTitle(QString("Duplicate \"%1\"").arg(m_snippet.name));
  }

private:
  SnippetDatabase::SerializedSnippet m_snippet;
};

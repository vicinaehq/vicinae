#pragma once
#include "navigation-controller.hpp"
#include "../image/url.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/form/form.hpp"
#include "ui/vertical-scroll-area/vertical-scroll-area.hpp"
#include "ui/views/base-view.hpp"
#include "ui/views/simple-view.hpp"
#include "utils/layout.hpp"
#include <qlogging.h>
#include <qtimer.h>

// For now this is just a FormView but with auto scrolling and focus
// We keep the old FormView for compatibility until we replace all
class ManagedFormView : public BaseView {
  FormWidget *m_form = new FormWidget;

public:
  FormWidget *form() const { return m_form; }

  virtual void onSubmit() = 0;

  /**
   * Customize submit action title
   */
  virtual QString submitTitle() const { return "Submit"; }

  void initialize() override final {
    initializeForm();

    auto panel = std::make_unique<FormActionPanelState>();
    auto section = panel->createSection();
    auto submit = new StaticAction(submitTitle(), ImageURL::builtin("enter-key"), [this]() { onSubmit(); });

    section->addAction(submit);
    setActions(std::move(panel));

    QTimer::singleShot(0, this, [this]() { m_form->focusFirst(); });
  }

  virtual void initializeForm() {}

  ManagedFormView(QWidget *parent = nullptr) : BaseView(parent) {
    auto scrollArea = new VerticalScrollArea;

    scrollArea->setWidget(m_form);
    VStack().add(scrollArea).imbue(this);
  }

  bool needsGlobalStatusBar() const override { return true; }
  bool supportsSearch() const override { return false; }
};

class FormView : public SimpleView {
public:
  virtual void onSubmit() = 0;

  /**
   * Customize submit action title
   */
  virtual QString submitTitle() const { return "Submit"; }

  void initialize() override final {
    initializeForm();
    auto panel = std::make_unique<FormActionPanelState>();
    auto section = panel->createSection();
    auto submit = new StaticAction(submitTitle(), ImageURL::builtin("enter-key"), [this]() { onSubmit(); });
    submit->setPrimary(true);
    section->addAction(submit);
    setActions(std::move(panel));
  }

  virtual void initializeForm() {}

  FormView(QWidget *parent = nullptr) : SimpleView(parent) {}

  bool needsGlobalStatusBar() const override { return true; }
  bool supportsSearch() const override { return false; }
};

#pragma once
#include "ui/action-pannel/action.hpp"

class ManageFallbackAction : public AbstractAction {
  void execute() override;

public:
  ManageFallbackAction();
};

class DisableFallbackAction : public AbstractAction {
  QString m_id;

  void execute() override;

public:
  DisableFallbackAction(const QString &id);
};

class MoveFallbackUpAction : public AbstractAction {
  QString m_id;

  void execute() override;

public:
  MoveFallbackUpAction(const QString &id);
};

class MoveFallbackDownAction : public AbstractAction {
  QString m_id;

  void execute() override;

public:
  MoveFallbackDownAction(const QString &id);
};

class EnableFallbackAction : public AbstractAction {
  QString m_id;

  void execute() override;

public:
  EnableFallbackAction(const QString &id);
};

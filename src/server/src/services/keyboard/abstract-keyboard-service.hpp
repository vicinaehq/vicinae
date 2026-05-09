#pragma once

class AbstractKeyboardService {
public:
  virtual ~AbstractKeyboardService() = default;
  virtual bool supportsKeyInjection() const = 0;
  virtual void paste(bool terminal) = 0;
  virtual void backspace(int n) = 0;
  virtual void moveCursorLeft(int n) = 0;
  virtual void space() = 0;
};

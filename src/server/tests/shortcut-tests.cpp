#include <QJsonArray>
#include <QJsonObject>
#include <QKeySequence>
#include <QVariantMap>
#include <catch2/catch_test_macros.hpp>
#include "extend/action-model.hpp"
#include "keyboard/keyboard.hpp"

namespace {

QVariantMap tokenAt(const QVariantList &tokens, qsizetype index) {
  REQUIRE(index >= 0);
  REQUIRE(index < tokens.size());
  return tokens.at(index).toMap();
}

} // namespace

TEST_CASE("shortcut tokens keep modifier and key semantics") {
  auto ctrlB = Keyboard::Shortcut(Qt::Key_B, Qt::ControlModifier).toDisplayTokens();
  REQUIRE(ctrlB.size() == 2);
  REQUIRE(tokenAt(ctrlB, 0).value("text").toString() == "Ctrl");
  REQUIRE(tokenAt(ctrlB, 1).value("text").toString() == "B");

  auto ctrlShiftB = Keyboard::Shortcut(Qt::Key_B, Qt::ControlModifier | Qt::ShiftModifier).toDisplayTokens();
  REQUIRE(ctrlShiftB.size() == 3);
  REQUIRE(tokenAt(ctrlShiftB, 0).value("text").toString() == "Ctrl");
  REQUIRE(tokenAt(ctrlShiftB, 1).value("icon").toString() == "keyboard-shift");
  REQUIRE(tokenAt(ctrlShiftB, 2).value("text").toString() == "B");

  auto shiftEnter = Keyboard::Shortcut::submit().toDisplayTokens();
  REQUIRE(shiftEnter.size() == 2);
  REQUIRE(tokenAt(shiftEnter, 0).value("icon").toString() == "keyboard-shift");
  REQUIRE(tokenAt(shiftEnter, 1).value("icon").toString() == "enter-key");
}

TEST_CASE("special keys are exposed as hybrid raycast tokens") {
  auto arrows = Keyboard::Shortcut(Qt::Key_Left).toDisplayTokens();
  REQUIRE(arrows.size() == 1);
  REQUIRE(tokenAt(arrows, 0).value("text").toString() == QStringLiteral("←"));

  auto tab = Keyboard::Shortcut(Qt::Key_Tab).toDisplayTokens();
  REQUIRE(tab.size() == 1);
  REQUIRE(tokenAt(tab, 0).value("icon").toString() == "tab-key");

  auto space = Keyboard::Shortcut(Qt::Key_Space).toDisplayTokens();
  REQUIRE(space.size() == 1);
  REQUIRE(tokenAt(space, 0).value("icon").toString() == "space-key");

  auto backspace = Keyboard::Shortcut(Qt::Key_Backspace).toDisplayTokens();
  REQUIRE(backspace.size() == 1);
  REQUIRE(tokenAt(backspace, 0).value("text").toString() == QStringLiteral("⌫"));

  auto del = Keyboard::Shortcut(Qt::Key_Delete).toDisplayTokens();
  REQUIRE(del.size() == 1);
  REQUIRE(tokenAt(del, 0).value("text").toString() == QStringLiteral("⌦"));
}

TEST_CASE("binding sequence stays separate from display tokens") {
  Keyboard::Shortcut const shortcut(Qt::Key_B, Qt::ControlModifier | Qt::ShiftModifier);

  REQUIRE(shortcut.toDisplayString() == "Ctrl+Shift+B");

  QKeySequence const expected(Qt::CTRL | Qt::SHIFT | Qt::Key_B);
  REQUIRE(shortcut.toBindingSequence() == expected.toString(QKeySequence::PortableText));
}

TEST_CASE("fromString parses plus key correctly") {
  auto plus = Keyboard::Shortcut::fromString("+");
  REQUIRE(plus.isValid());
  REQUIRE(plus.key() == Qt::Key_Plus);
  REQUIRE(plus.mods() == Qt::KeyboardModifiers{});

  auto ctrlPlus = Keyboard::Shortcut::fromString("ctrl++");
  REQUIRE(ctrlPlus.isValid());
  REQUIRE(ctrlPlus.key() == Qt::Key_Plus);
  REQUIRE(ctrlPlus.mods().testFlag(Qt::ControlModifier));

  auto ctrlShiftPlus = Keyboard::Shortcut::fromString("ctrl+shift++");
  REQUIRE(ctrlShiftPlus.isValid());
  REQUIRE(ctrlShiftPlus.key() == Qt::Key_Plus);
  REQUIRE(ctrlShiftPlus.mods().testFlag(Qt::ControlModifier));
  REQUIRE(ctrlShiftPlus.mods().testFlag(Qt::ShiftModifier));
}

TEST_CASE("extension object shortcuts can represent plus keys") {
  QJsonObject shortcut{
      {QStringLiteral("key"), QStringLiteral("+")},
      {QStringLiteral("modifiers"), QJsonArray{QStringLiteral("ctrl"), QStringLiteral("shift")}},
  };

  auto parsed = ActionPannelParser::parseKeyboardShortcut(shortcut);
  REQUIRE(parsed.isValid());
  REQUIRE(parsed.key() == Qt::Key_Plus);
  REQUIRE(parsed.mods().testFlag(Qt::ControlModifier));
  REQUIRE(parsed.mods().testFlag(Qt::ShiftModifier));
}

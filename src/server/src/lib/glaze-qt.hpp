#pragma once
#include <QDateTime>
#include <QString>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <string_view>

namespace glz {

template <> struct from<JSON, QString> {
  template <auto Opts> static void op(QString &value, is_context auto &&ctx, auto &&it, auto &&end) {
    if (*it == 'n') {
      match<"null", Opts>(ctx, it, end);
      value = QString();
      return;
    }
    std::string_view sv;
    parse<JSON>::op<Opts>(sv, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value = QString::fromUtf8(sv.data(), static_cast<qsizetype>(sv.size()));
  }
};

template <> struct to<JSON, QString> {
  template <auto Opts>
  static void op(const QString &value, is_context auto &&ctx, auto &&b, auto &&ix) noexcept {
    const auto utf8 = value.toUtf8();
    const std::string_view sv(utf8.constData(), utf8.size());
    serialize<JSON>::op<Opts>(sv, ctx, b, ix);
  }
};

template <> struct from<JSON, QDateTime> {
  template <auto Opts> static void op(QDateTime &value, is_context auto &&ctx, auto &&it, auto &&end) {
    QString str;
    from<JSON, QString>::op<Opts>(str, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value = QDateTime::fromString(str, Qt::ISODate);
  }
};

template <> struct to<JSON, QDateTime> {
  template <auto Opts>
  static void op(const QDateTime &value, is_context auto &&ctx, auto &&b, auto &&ix) noexcept {
    const auto str = value.toString(Qt::ISODate);
    to<JSON, QString>::op<Opts>(str, ctx, b, ix);
  }
};

} // namespace glz

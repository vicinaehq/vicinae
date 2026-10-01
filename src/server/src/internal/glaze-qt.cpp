#include "glaze-qt.hpp"
#include <glaze/json/generic.hpp>
#include <qjsonvalue.h>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>

QJsonValue glazeToQJsonValue(const glz::generic &v) {
  if (v.is_boolean()) return v.get_boolean();
  if (v.is_string()) return v.get_string().c_str();
  if (v.is_number()) return v.get_number();
  if (v.is_null()) return QJsonValue::Null;
  if (v.is_array())
    return v.get_array() | std::views::transform(glazeToQJsonValue) | std::ranges::to<QJsonArray>();
  if (v.is_object()) return glazeToQJsonObject(v.get_object());
  return QJsonValue::Undefined;
}

QJsonObject glazeToQJsonObject(const glz::generic::object_t &v) {
  QJsonObject obj;

  for (const auto &[key, v] : v) {
    obj[key.c_str()] = glazeToQJsonValue(v);
  }

  return obj;
}

glz::generic::object_t qJsonObjectToGlazeGeneric(const QJsonObject &v) {
  glz::generic::object_t obj;

  for (const auto &key : v.keys()) {
    obj[key.toStdString()] = qJsonValueToGlazeGeneric(v.value(key));
  }

  return obj;
}

glz::generic qJsonValueToGlazeGeneric(const QJsonValue &v) {
  if (v.isBool()) return v.toBool();
  if (v.isString()) return v.toString().toStdString();
  if (v.isDouble()) return v.toDouble();
  if (v.isNull()) return glz::generic::null_t{};
  if (v.isArray())
    return v.toArray() | std::views::transform(qJsonValueToGlazeGeneric) |
           std::ranges::to<glz::generic::array_t>();
  if (v.isObject()) return qJsonObjectToGlazeGeneric(v.toObject());
  return {};
}

QVariant glazeToQVariant(const glz::generic &v) {
  if (v.is_boolean()) return v.get_boolean();
  if (v.is_string()) return QString::fromStdString(v.get_string());
  if (v.is_number()) return v.get_number();
  if (v.is_array()) {
    QVariantList list;
    list.reserve(static_cast<qsizetype>(v.get_array().size()));
    for (const auto &item : v.get_array()) {
      list.append(glazeToQVariant(item));
    }
    return list;
  }
  if (v.is_object()) {
    QVariantMap map;
    for (const auto &[key, item] : v.get_object()) {
      map.insert(QString::fromStdString(key), glazeToQVariant(item));
    }
    return map;
  }
  return {};
}

glz::generic qVariantToGlazeGeneric(const QVariant &v) {
  switch (v.typeId()) {
  case QMetaType::Bool:
    return v.toBool();
  case QMetaType::QString:
    return v.toString().toStdString();
  case QMetaType::Int:
  case QMetaType::UInt:
  case QMetaType::LongLong:
  case QMetaType::ULongLong:
  case QMetaType::Double:
  case QMetaType::Float:
    return v.toDouble();
  case QMetaType::QStringList:
  case QMetaType::QVariantList: {
    glz::generic::array_t array;
    for (const auto &item : v.toList()) {
      array.emplace_back(qVariantToGlazeGeneric(item));
    }
    return array;
  }
  case QMetaType::QVariantMap: {
    glz::generic::object_t object;
    for (auto [key, item] : v.toMap().asKeyValueRange()) {
      object[key.toStdString()] = qVariantToGlazeGeneric(item);
    }
    return object;
  }
  default:
    return {};
  }
}

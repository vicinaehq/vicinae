#include "extend/model-deser.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#include <QColor>
#include <QJsonDocument>
#include <glaze/json.hpp>

#include "extend/action-model.hpp"
#include "extend/color-model.hpp"
#include "extend/detail-model.hpp"
#include "extend/dropdown-model.hpp"
#include "extend/empty-view-model.hpp"
#include "extend/event-counted.hpp"
#include "extend/form-model.hpp"
#include "extend/grid-model.hpp"
#include "extend/image-model.hpp"
#include "extend/list-model.hpp"
#include "extend/metadata-model.hpp"
#include "extend/model-parser.hpp"
#include "extend/pagination-model.hpp"
#include "extend/root-detail-model.hpp"
#include "extend/tag-model.hpp"
#include "glaze-qt.hpp"
#include "lib/keyboard/keyboard.hpp"
#include "theme.hpp"
#include "ui/image/url.hpp"
#include "ui/omni-painter/omni-painter.hpp"

// ============================================================
// Tag constant and parse options
// ============================================================

static constexpr std::string_view TAG = "$t";
static constexpr glz::opts PARSE_OPTS{.error_on_unknown_keys = false, .minified = true};

// ============================================================
// QString conversion helpers — used only at model boundaries
// ============================================================

static inline QString toQStr(const std::string &s) {
  return QString::fromUtf8(s.data(), static_cast<qsizetype>(s.size()));
}

static inline std::optional<QString> toOptQStr(const std::optional<std::string> &s) {
  return s ? std::optional(toQStr(*s)) : std::nullopt;
}

// ============================================================
// Wire structs — all strings are std::string to avoid
// UTF-8 → UTF-16 conversion during the Glaze parse phase.
// Conversion to QString happens in toModel() / from<JSON> boundaries.
// ============================================================

struct DynColorRaw {
  std::string light;
  std::string dark;
  bool adjustContrast = true;
};

template <> struct glz::meta<DynColorRaw> {
  using T = DynColorRaw;
  static constexpr auto value =
      glz::object("light", &T::light, "dark", &T::dark, "adjustContrast", &T::adjustContrast);
};

struct ColorLikeWire {
  std::optional<std::string> raw;
  std::optional<DynColorRaw> dynamic;
};

template <> struct glz::meta<ColorLikeWire> {
  using T = ColorLikeWire;
  static constexpr auto value = glz::object("raw", &T::raw, "dynamic", &T::dynamic);
};

struct ThemedIconSourceWire {
  std::string light;
  std::string dark;
};

template <> struct glz::meta<ThemedIconSourceWire> {
  using T = ThemedIconSourceWire;
  static constexpr auto value = glz::object("light", &T::light, "dark", &T::dark);
};

struct ImageSourceWire {
  std::optional<std::string> raw;
  std::optional<ThemedIconSourceWire> themed;
  std::optional<std::string> light;
  std::optional<std::string> dark;
};

template <> struct glz::meta<ImageSourceWire> {
  using T = ImageSourceWire;
  static constexpr auto value =
      glz::object("raw", &T::raw, "themed", &T::themed, "light", &T::light, "dark", &T::dark);
};

struct ImageObjectWire {
  std::optional<std::variant<QString, ThemedIconSource>> source;
  std::optional<std::variant<QString, ThemedIconSource>> fallback;
  std::optional<ColorLike> tintColor;
  std::optional<std::string> mask;
  std::optional<std::string> fileIcon;
};

template <> struct glz::meta<ImageObjectWire> {
  using T = ImageObjectWire;
  static constexpr auto value = glz::object("source", &T::source, "fallback", &T::fallback, "tintColor",
                                            &T::tintColor, "mask", &T::mask, "fileIcon", &T::fileIcon);
};

struct FlexText {
  std::string value;
  std::optional<ColorLike> color;
};

struct FlexTextObj {
  std::string value;
  std::optional<ColorLike> color;
};

template <> struct glz::meta<FlexTextObj> {
  using T = FlexTextObj;
  static constexpr auto value = glz::object("value", &T::value, "color", &T::color);
};

namespace glz {

template <> struct from<JSON, FlexText> {
  template <auto Opts, auto... Extra>
  static void op(FlexText &v, is_context auto &&ctx, auto &&it, auto &&end) {
    if (*it == '"') {
      parse<JSON>::op<Opts>(v.value, ctx, it, end);
    } else if (*it == '{') {
      FlexTextObj w;
      parse<JSON>::op<Opts>(w, ctx, it, end);
      if (bool(ctx.error)) [[unlikely]]
        return;
      v.value = std::move(w.value);
      v.color = std::move(w.color);
    } else {
      raw_json skip;
      parse<JSON>::op<Opts>(skip, ctx, it, end);
    }
  }
};

} // namespace glz

// ============================================================
// Custom glz::from specializations for polymorphic types
// ============================================================

namespace glz {

// --- ThemedIconSource ---
template <> struct from<JSON, ThemedIconSource> {
  template <auto Opts, auto... Extra>
  static void op(ThemedIconSource &value, is_context auto &&ctx, auto &&it, auto &&end) {
    ThemedIconSourceWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value.light = toQStr(w.light);
    value.dark = toQStr(w.dark);
  }
};

// --- ColorLike ---
template <> struct from<JSON, ColorLike> {
  template <auto Opts, auto... Extra>
  static void op(ColorLike &value, is_context auto &&ctx, auto &&it, auto &&end) {
    if (*it != '{') {
      ctx.error = error_code::syntax_error;
      return;
    }
    ColorLikeWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;

    if (w.raw) {
      auto qs = toQStr(*w.raw);
      if (auto tint = ImageURL::tintForName(qs); tint != SemanticColor::InvalidTint) {
        value = tint;
      } else {
        value = std::move(qs);
      }
    } else if (w.dynamic) {
      value = DynamicColor{.light = toQStr(w.dynamic->light),
                           .dark = toQStr(w.dynamic->dark),
                           .adjustContrast = w.dynamic->adjustContrast};
    }
  }
};

// --- variant<QString, ThemedIconSource> (image source) ---
template <> struct from<JSON, std::variant<QString, ThemedIconSource>> {
  template <auto Opts, auto... Extra>
  static void op(std::variant<QString, ThemedIconSource> &value, is_context auto &&ctx, auto &&it,
                 auto &&end) {
    if (*it == '"') {
      std::string s;
      parse<JSON>::op<Opts>(s, ctx, it, end);
      if (bool(ctx.error)) [[unlikely]]
        return;
      value = toQStr(s);
      return;
    }
    if (*it == '{') {
      ImageSourceWire w;
      parse<JSON>::op<Opts>(w, ctx, it, end);
      if (bool(ctx.error)) [[unlikely]]
        return;
      if (w.raw) {
        value = toQStr(*w.raw);
      } else if (w.themed) {
        ThemedIconSource themed;
        themed.light = toQStr(w.themed->light);
        themed.dark = toQStr(w.themed->dark);
        value = std::move(themed);
      } else if (w.light) {
        ThemedIconSource themed;
        themed.light = toQStr(*w.light);
        if (w.dark) themed.dark = toQStr(*w.dark);
        value = std::move(themed);
      }
      return;
    }
    value = QString();
  }
};

// --- ImageLikeModel ---
template <> struct from<JSON, ImageLikeModel> {
  template <auto Opts, auto... Extra>
  static void op(ImageLikeModel &value, is_context auto &&ctx, auto &&it, auto &&end) {
    if (*it == '"') {
      std::string s;
      parse<JSON>::op<Opts>(s, ctx, it, end);
      if (bool(ctx.error)) [[unlikely]]
        return;
      value = ExtensionImageModel{.source = toQStr(s)};
      return;
    }
    if (*it == '{') {
      ImageObjectWire w;
      parse<JSON>::op<Opts>(w, ctx, it, end);
      if (bool(ctx.error)) [[unlikely]]
        return;

      if (w.source) {
        ExtensionImageModel model;
        model.source = std::move(*w.source);
        if (w.fallback) model.fallback = std::move(*w.fallback);
        if (w.tintColor) model.tintColor = std::move(*w.tintColor);
        if (w.mask) model.mask = OmniPainter::maskForName(toQStr(*w.mask));
        value = std::move(model);
      } else if (w.fileIcon) {
        value = ExtensionFileIconModel{.file = *w.fileIcon};
      } else {
        value = InvalidImageModel{};
      }
      return;
    }
    value = InvalidImageModel{};
  }
};

// --- Keyboard::Shortcut ---
template <> struct from<JSON, Keyboard::Shortcut> {
  template <auto Opts, auto... Extra>
  static void op(Keyboard::Shortcut &value, is_context auto &&ctx, auto &&it, auto &&end) {
    static const std::unordered_map<std::string, Keybind> NAMED_MAP = {
        {"copy", Keybind::CopyAction},
        {"copy-deeplink", Keybind::CopyAction},
        {"copy-name", Keybind::CopyNameAction},
        {"copy-path", Keybind::CopyPathAction},
        {"save", Keybind::SaveAction},
        {"duplicate", Keybind::DuplicateAction},
        {"edit", Keybind::EditAction},
        {"move-down", Keybind::MoveDownAction},
        {"move-up", Keybind::MoveUpAction},
        {"new", Keybind::NewAction},
        {"open", Keybind::OpenAction},
        {"open-with", Keybind::OpenAction},
        {"pin", Keybind::PinAction},
        {"refresh", Keybind::RefreshAction},
        {"remove", Keybind::RemoveAction},
        {"remove-all", Keybind::DangerousRemoveAction},
    };

    if (*it == '"') {
      std::string s;
      parse<JSON>::op<Opts>(s, ctx, it, end);
      if (bool(ctx.error)) [[unlikely]]
        return;
      if (auto found = NAMED_MAP.find(s); found != NAMED_MAP.end()) {
        value = found->second;
      } else {
        value = Keyboard::Shortcut::fromString(toQStr(s));
      }
      return;
    }
    if (*it == '{') {
      struct Obj {
        std::string key;
        std::vector<std::string> modifiers;
      };
      Obj obj;
      parse<JSON>::op<Opts>(obj, ctx, it, end);
      if (bool(ctx.error)) [[unlikely]]
        return;

      auto k = Keyboard::keyFromString(toQStr(obj.key));
      if (!k) return;

      Qt::KeyboardModifiers mods{};
      for (const auto &m : obj.modifiers) {
        auto mod = Keyboard::modifierFromString(toQStr(m));
        if (mod) mods.setFlag(*mod);
      }
      value = Keyboard::Shortcut(*k, mods);
      return;
    }
    value = {};
  }
};

// --- QJsonObject ---
template <> struct from<JSON, QJsonObject> {
  template <auto Opts, auto... Extra>
  static void op(QJsonObject &value, is_context auto &&ctx, auto &&it, auto &&end) {
    raw_json raw;
    parse<JSON>::op<Opts>(raw, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    auto doc = QJsonDocument::fromJson(QByteArray::fromRawData(raw.str.data(), raw.str.size()));
    value = doc.object();
  }
};

// --- ObjectFit ---
template <> struct from<JSON, ObjectFit> {
  template <auto Opts, auto... Extra>
  static void op(ObjectFit &value, is_context auto &&ctx, auto &&it, auto &&end) {
    std::string s;
    parse<JSON>::op<Opts>(s, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value = (s == "fill") ? ObjectFit::Fill : ObjectFit::Contain;
  }
};

// --- GridInset ---
template <> struct from<JSON, GridInset> {
  template <auto Opts, auto... Extra>
  static void op(GridInset &value, is_context auto &&ctx, auto &&it, auto &&end) {
    std::string s;
    parse<JSON>::op<Opts>(s, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    if (s == "zero")
      value = GridInset::None;
    else if (s == "small")
      value = GridInset::Small;
    else if (s == "medium")
      value = GridInset::Medium;
    else if (s == "large")
      value = GridInset::Large;
    else
      value = GridInset::Small;
  }
};

// --- OmniPainter::ImageMaskType ---
template <> struct from<JSON, OmniPainter::ImageMaskType> {
  template <auto Opts>
  static void op(OmniPainter::ImageMaskType &value, is_context auto &&ctx, auto &&it, auto &&end) {
    std::string s;
    parse<JSON>::op<Opts>(s, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value = OmniPainter::maskForName(toQStr(s));
  }
};

// --- DropdownModel::Filtering ---
template <> struct from<JSON, DropdownModel::Filtering> {
  template <auto Opts, auto... Extra>
  static void op(DropdownModel::Filtering &value, is_context auto &&ctx, auto &&it, auto &&end) {
    if (*it == 't' || *it == 'f') {
      bool b = true;
      parse<JSON>::op<Opts>(b, ctx, it, end);
      value = {.keepSectionOrder = true, .enabled = b};
      return;
    }
    value = {.keepSectionOrder = true, .enabled = true};
    raw_json skip;
    parse<JSON>::op<Opts>(skip, ctx, it, end);
  }
};

// --- variant<bool, ActionPannelSubmenuFiltering> ---
template <> struct from<JSON, std::variant<bool, ActionPannelSubmenuFiltering>> {
  template <auto Opts>
  static void op(std::variant<bool, ActionPannelSubmenuFiltering> &value, is_context auto &&ctx, auto &&it,
                 auto &&end) {
    if (*it == 't' || *it == 'f') {
      bool b = false;
      parse<JSON>::op<Opts>(b, ctx, it, end);
      value = b;
    } else if (*it == '{') {
      ActionPannelSubmenuFiltering f;
      parse<JSON>::op<Opts>(f, ctx, it, end);
      value = std::move(f);
    } else {
      raw_json skip;
      parse<JSON>::op<Opts>(skip, ctx, it, end);
    }
  }
};

// --- AccessoryModel ---

} // namespace glz

struct AccessoryWire {
  std::optional<ImageLikeModel> icon;
  std::optional<std::string> tooltip;
  std::optional<FlexText> tag;
  std::optional<FlexText> text;
};

template <> struct glz::meta<AccessoryWire> {
  using T = AccessoryWire;
  static constexpr auto value =
      glz::object("icon", &T::icon, "tooltip", &T::tooltip, "tag", &T::tag, "text", &T::text);
};

namespace glz {

template <> struct from<JSON, AccessoryModel> {
  template <auto Opts, auto... Extra>
  static void op(AccessoryModel &value, is_context auto &&ctx, auto &&it, auto &&end) {
    if (*it != '{') {
      raw_json skip;
      parse<JSON>::op<Opts>(skip, ctx, it, end);
      return;
    }
    AccessoryWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;

    if (w.icon) value.icon = std::move(*w.icon);
    if (w.tooltip) value.tooltip = toQStr(*w.tooltip);
    if (w.tag) {
      value.data = AccessoryModel::Tag{.color = std::move(w.tag->color), .value = toQStr(w.tag->value)};
    } else if (w.text) {
      value.data = AccessoryModel::Text{.color = std::move(w.text->color), .value = toQStr(w.text->value)};
    }
  }
};

// --- ImageURL ---
template <> struct from<JSON, ImageURL> {
  template <auto Opts, auto... Extra>
  static void op(ImageURL &value, is_context auto &&ctx, auto &&it, auto &&end) {
    ImageLikeModel img;
    from<JSON, ImageLikeModel>::op<Opts>(img, ctx, it, end);
    if (!bool(ctx.error)) value = ImageURL(img);
  }
};

} // namespace glz

// ============================================================
// Helper types for fields with non-trivial wire shapes
// ============================================================

struct FlexString {
  std::string value;
};

namespace glz {
template <> struct from<JSON, FlexString> {
  template <auto Opts, auto... Extra>
  static void op(FlexString &v, is_context auto &&ctx, auto &&it, auto &&end) {
    if (*it == '"') {
      parse<JSON>::op<Opts>(v.value, ctx, it, end);
    } else if (*it == '{') {
      struct W {
        std::string value;
      };
      W w;
      parse<JSON>::op<Opts>(w, ctx, it, end);
      v.value = std::move(w.value);
    } else {
      raw_json skip;
      parse<JSON>::op<Opts>(skip, ctx, it, end);
    }
  }
};
} // namespace glz

struct GridContentParsed {
  GridItemViewModel::Content content = InvalidImageModel{};
  std::optional<std::string> tooltip;
};

struct GridContentObj {
  std::optional<std::string> tooltip;
  std::optional<ColorLike> color;
  std::optional<ImageLikeModel> value;
  std::optional<std::variant<QString, ThemedIconSource>> source;
  std::optional<std::variant<QString, ThemedIconSource>> fallback;
  std::optional<ColorLike> tintColor;
  std::optional<std::string> mask;
  std::optional<std::string> fileIcon;
};

template <> struct glz::meta<GridContentObj> {
  using T = GridContentObj;
  static constexpr auto value = glz::object("tooltip", &T::tooltip, "color", &T::color, "value", &T::value,
                                            "source", &T::source, "fallback", &T::fallback, "tintColor",
                                            &T::tintColor, "mask", &T::mask, "fileIcon", &T::fileIcon);
};

namespace glz {
template <> struct from<JSON, GridContentParsed> {
  template <auto Opts, auto... Extra>
  static void op(GridContentParsed &v, is_context auto &&ctx, auto &&it, auto &&end) {
    if (*it == '"') {
      ImageLikeModel img;
      from<JSON, ImageLikeModel>::op<Opts>(img, ctx, it, end);
      v.content = std::move(img);
      return;
    }
    if (*it == '{') {
      GridContentObj w;
      parse<JSON>::op<Opts>(w, ctx, it, end);
      if (bool(ctx.error)) [[unlikely]]
        return;
      if (w.tooltip) v.tooltip = std::move(*w.tooltip);
      if (w.color) {
        v.content = std::move(*w.color);
      } else if (w.value) {
        v.content = std::move(*w.value);
      } else if (w.source) {
        ExtensionImageModel model;
        model.source = std::move(*w.source);
        if (w.fallback) model.fallback = std::move(*w.fallback);
        if (w.tintColor) model.tintColor = std::move(*w.tintColor);
        if (w.mask) model.mask = OmniPainter::maskForName(toQStr(*w.mask));
        v.content = std::move(model);
      } else if (w.fileIcon) {
        v.content = ExtensionFileIconModel{.file = *w.fileIcon};
      } else {
        v.content = InvalidImageModel{};
      }
      return;
    }
    raw_json skip;
    parse<JSON>::op<Opts>(skip, ctx, it, end);
  }
};
} // namespace glz

struct ImageLikeWrapped {
  ImageLikeModel value = InvalidImageModel{};
};

struct ImageWrappedObj {
  std::optional<ImageLikeModel> value;
  std::optional<std::variant<QString, ThemedIconSource>> source;
  std::optional<std::variant<QString, ThemedIconSource>> fallback;
  std::optional<ColorLike> tintColor;
  std::optional<std::string> mask;
  std::optional<std::string> fileIcon;
};

template <> struct glz::meta<ImageWrappedObj> {
  using T = ImageWrappedObj;
  static constexpr auto value =
      glz::object("value", &T::value, "source", &T::source, "fallback", &T::fallback, "tintColor",
                  &T::tintColor, "mask", &T::mask, "fileIcon", &T::fileIcon);
};

namespace glz {
template <> struct from<JSON, ImageLikeWrapped> {
  template <auto Opts, auto... Extra>
  static void op(ImageLikeWrapped &v, is_context auto &&ctx, auto &&it, auto &&end) {
    if (*it == '"') {
      from<JSON, ImageLikeModel>::op<Opts>(v.value, ctx, it, end);
      return;
    }
    if (*it == '{') {
      ImageWrappedObj w;
      parse<JSON>::op<Opts>(w, ctx, it, end);
      if (bool(ctx.error)) [[unlikely]]
        return;
      if (w.value && !w.source && !w.fileIcon) {
        v.value = std::move(*w.value);
      } else if (w.source) {
        ExtensionImageModel model;
        model.source = std::move(*w.source);
        if (w.fallback) model.fallback = std::move(*w.fallback);
        if (w.tintColor) model.tintColor = std::move(*w.tintColor);
        if (w.mask) model.mask = OmniPainter::maskForName(toQStr(*w.mask));
        v.value = std::move(model);
      } else if (w.fileIcon) {
        v.value = ExtensionFileIconModel{.file = *w.fileIcon};
      } else {
      }
      return;
    }
    raw_json skip;
    parse<JSON>::op<Opts>(skip, ctx, it, end);
  }
};
} // namespace glz

// ============================================================
// glz::meta for simple/leaf types (kept for compatibility)
// ============================================================

template <> struct glz::meta<ThemedIconSource> {
  using T = ThemedIconSource;
  static constexpr auto value = glz::object("light", &T::light, "dark", &T::dark);
};

template <> struct glz::meta<ActionPannelSubmenuFiltering> {
  using T = ActionPannelSubmenuFiltering;
  static constexpr auto value = glz::object("keepSectionOrder", &T::keepSectionOrder);
};

template <> struct glz::meta<PaginationModel> {
  using T = PaginationModel;
  static constexpr auto value =
      glz::object("onLoadMore", &T::onLoadMore, "hasMore", &T::hasMore, "pageSize", &T::pageSize);
};

template <typename T> struct glz::meta<EventCounted<T>> {
  using U = EventCounted<T>;
  static constexpr auto value = glz::object("value", &U::value, "eventCount", &U::eventCount);
};

// ============================================================
// Wire structs + from<JSON> for model types with QString fields
// ============================================================

struct MetadataLinkWire {
  std::string title;
  std::string text;
  std::string target;
};

template <> struct glz::meta<MetadataLinkWire> {
  using T = MetadataLinkWire;
  static constexpr auto value = glz::object("title", &T::title, "text", &T::text, "target", &T::target);
};

template <> struct glz::meta<MetadataLink> {
  using T = MetadataLink;
  static constexpr auto value = glz::object("title", &T::title, "text", &T::text, "target", &T::target);
};

namespace glz {
template <> struct from<JSON, MetadataLink> {
  template <auto Opts, auto... Extra>
  static void op(MetadataLink &value, is_context auto &&ctx, auto &&it, auto &&end) {
    MetadataLinkWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value.title = toQStr(w.title);
    value.text = toQStr(w.text);
    value.target = toQStr(w.target);
  }
};
} // namespace glz

struct ActionModelWire {
  std::string title;
  std::string onAction;
  std::optional<std::string> onSubmit;
  std::optional<ImageLikeModel> icon;
  std::optional<Keyboard::Shortcut> shortcut;
  std::string type;
  QJsonObject quicklink;
  std::optional<std::string> stableId;
};

template <> struct glz::meta<ActionModelWire> {
  using T = ActionModelWire;
  static constexpr auto value = glz::object("title", &T::title, "onAction", &T::onAction, "onSubmit",
                                            &T::onSubmit, "icon", &T::icon, "shortcut", &T::shortcut, "type",
                                            &T::type, "quicklink", &T::quicklink, "stableId", &T::stableId);
};

template <> struct glz::meta<ActionModel> {
  using T = ActionModel;
  static constexpr auto value = glz::object("title", &T::title, "onAction", &T::onAction, "onSubmit",
                                            &T::onSubmit, "icon", &T::icon, "shortcut", &T::shortcut, "type",
                                            &T::type, "quicklink", &T::quicklink, "stableId", &T::stableId);
};

namespace glz {
template <> struct from<JSON, ActionModel> {
  template <auto Opts, auto... Extra>
  static void op(ActionModel &value, is_context auto &&ctx, auto &&it, auto &&end) {
    ActionModelWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value.title = toQStr(w.title);
    value.onAction = toQStr(w.onAction);
    value.onSubmit = toOptQStr(w.onSubmit);
    value.icon = std::move(w.icon);
    value.shortcut = std::move(w.shortcut);
    value.type = toQStr(w.type);
    value.quicklink = std::move(w.quicklink);
    value.stableId = toOptQStr(w.stableId);
  }
};
} // namespace glz

struct TagItemModelWire {
  std::string text;
  std::string onAction;
  std::optional<ImageLikeModel> icon;
  std::optional<ColorLike> color;
};

template <> struct glz::meta<TagItemModelWire> {
  using T = TagItemModelWire;
  static constexpr auto value =
      glz::object("text", &T::text, "onAction", &T::onAction, "icon", &T::icon, "color", &T::color);
};

template <> struct glz::meta<TagItemModel> {
  using T = TagItemModel;
  static constexpr auto value =
      glz::object("text", &T::text, "onAction", &T::onAction, "icon", &T::icon, "color", &T::color);
};

namespace glz {
template <> struct from<JSON, TagItemModel> {
  template <auto Opts, auto... Extra>
  static void op(TagItemModel &value, is_context auto &&ctx, auto &&it, auto &&end) {
    TagItemModelWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value.text = toQStr(w.text);
    value.onAction = toQStr(w.onAction);
    value.icon = std::move(w.icon);
    value.color = std::move(w.color);
  }
};
} // namespace glz

struct DropdownItemWire {
  std::string title;
  std::string value;
  std::vector<std::string> keywords;
  std::optional<ImageLikeModel> icon;
};

template <> struct glz::meta<DropdownItemWire> {
  using T = DropdownItemWire;
  static constexpr auto value =
      glz::object("title", &T::title, "value", &T::value, "keywords", &T::keywords, "icon", &T::icon);
};

template <> struct glz::meta<DropdownModel::Item> {
  using T = DropdownModel::Item;
  static constexpr auto value =
      glz::object("title", &T::title, "value", &T::value, "keywords", &T::keywords, "icon", &T::icon);
};

namespace glz {
template <> struct from<JSON, DropdownModel::Item> {
  template <auto Opts, auto... Extra>
  static void op(DropdownModel::Item &value, is_context auto &&ctx, auto &&it, auto &&end) {
    DropdownItemWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value.title = toQStr(w.title);
    value.value = toQStr(w.value);
    value.keywords.reserve(w.keywords.size());
    for (auto &kw : w.keywords)
      value.keywords.emplace_back(toQStr(kw));
    value.icon = std::move(w.icon);
  }
};
} // namespace glz

// ============================================================
// Wire struct forward declarations
// ============================================================

struct ActionPannelSectionModelWire;
struct ActionPannelSubmenuModelWire;

// ============================================================
// Child variant types with tagged variant metas
// ============================================================

using APSectionChild = std::variant<ActionModel, std::shared_ptr<ActionPannelSubmenuModelWire>>;

template <> struct glz::meta<APSectionChild> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids = std::array{"action", "action-panel-submenu"};
};

using APSubmenuChild = std::variant<std::shared_ptr<ActionPannelSectionModelWire>, ActionModel,
                                    std::shared_ptr<ActionPannelSubmenuModelWire>>;

template <> struct glz::meta<APSubmenuChild> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids = std::array{"action-panel-section", "action", "action-panel-submenu"};
};

using APPanelChild = std::variant<ActionModel, std::shared_ptr<ActionPannelSectionModelWire>,
                                  std::shared_ptr<ActionPannelSubmenuModelWire>>;

template <> struct glz::meta<APPanelChild> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids = std::array{"action", "action-panel-section", "action-panel-submenu"};
};

// ============================================================
// Wire structs for action panel (mutually recursive via shared_ptr)
// ============================================================

struct ActionPannelSectionModelWire {
  std::string title;
  std::vector<APSectionChild> children;
};

template <> struct glz::meta<ActionPannelSectionModelWire> {
  using T = ActionPannelSectionModelWire;
  static constexpr auto value = glz::object("title", &T::title, "children", &T::children);
};

struct ActionPannelSubmenuModelWire {
  std::string title;
  std::optional<ImageLikeModel> icon;
  std::optional<Keyboard::Shortcut> shortcut;
  std::optional<bool> autoFocus;
  std::optional<std::variant<bool, ActionPannelSubmenuFiltering>> filtering;
  std::optional<bool> isLoading;
  std::optional<bool> throttle;
  std::string onOpen;
  std::string onSearchTextChange;
  std::optional<std::string> stableId;
  std::vector<APSubmenuChild> children;
};

template <> struct glz::meta<ActionPannelSubmenuModelWire> {
  using T = ActionPannelSubmenuModelWire;
  static constexpr auto value = glz::object(
      "title", &T::title, "icon", &T::icon, "shortcut", &T::shortcut, "autoFocus", &T::autoFocus, "filtering",
      &T::filtering, "isLoading", &T::isLoading, "throttle", &T::throttle, "onOpen", &T::onOpen,
      "onSearchTextChange", &T::onSearchTextChange, "stableId", &T::stableId, "children", &T::children);
};

struct ActionPannelModelWire {
  std::string title;
  std::optional<std::string> stableId;
  std::vector<APPanelChild> children;
};

template <> struct glz::meta<ActionPannelModelWire> {
  using T = ActionPannelModelWire;
  static constexpr auto value =
      glz::object("title", &T::title, "stableId", &T::stableId, "children", &T::children);
};

// ============================================================
// Distribute helpers for action panel wire → model conversion
// ============================================================

static void fixupAction(ActionModel &a) {
  if (a.type.isEmpty()) a.type = QStringLiteral("callback");
}

static ActionPannelSectionModel toModel(const ActionPannelSectionModelWire &w);
static ActionPannelSubmenuModel toModel(const ActionPannelSubmenuModelWire &w);

static ActionPannelSectionModel toModel(const ActionPannelSectionModelWire &w) {
  ActionPannelSectionModel m;
  m.title = toQStr(w.title);
  m.items.reserve(w.children.size());
  for (const auto &child : w.children) {
    std::visit(
        [&](auto &&c) {
          using T = std::decay_t<decltype(c)>;
          if constexpr (std::is_same_v<T, ActionModel>) {
            auto a = c;
            fixupAction(a);
            m.items.push_back(std::move(a));
          } else if constexpr (std::is_same_v<T, std::shared_ptr<ActionPannelSubmenuModelWire>>) {
            auto sub = std::make_shared<ActionPannelSubmenuModel>(toModel(*c));
            m.items.push_back(std::move(sub));
          }
        },
        child);
  }
  return m;
}

static ActionPannelSubmenuModel toModel(const ActionPannelSubmenuModelWire &w) {
  ActionPannelSubmenuModel m;
  m.title = toQStr(w.title);
  m.icon = w.icon;
  m.shortcut = w.shortcut;
  m.autoFocus = w.autoFocus;
  m.filtering = w.filtering;
  m.isLoading = w.isLoading;
  m.throttle = w.throttle;
  m.onOpen = toQStr(w.onOpen);
  m.onSearchTextChange = toQStr(w.onSearchTextChange);
  m.stableId = toOptQStr(w.stableId);
  m.children.reserve(w.children.size());

  for (const auto &child : w.children) {
    std::visit(
        [&](auto &&c) {
          using T = std::decay_t<decltype(c)>;
          if constexpr (std::is_same_v<T, ActionModel>) {
            auto a = c;
            fixupAction(a);
            m.children.push_back(std::move(a));
          } else if constexpr (std::is_same_v<T, std::shared_ptr<ActionPannelSectionModelWire>>) {
            auto sec = std::make_shared<ActionPannelSectionModel>(toModel(*c));
            m.children.push_back(std::move(sec));
          } else if constexpr (std::is_same_v<T, std::shared_ptr<ActionPannelSubmenuModelWire>>) {
            auto sub = std::make_shared<ActionPannelSubmenuModel>(toModel(*c));
            m.children.push_back(std::move(sub));
          }
        },
        child);
  }
  return m;
}

static ActionPannelModel toModel(const ActionPannelModelWire &w) {
  ActionPannelModel m;
  m.dirty = true;
  m.title = toQStr(w.title);
  m.stableId = toOptQStr(w.stableId);
  m.children.reserve(w.children.size());

  for (const auto &child : w.children) {
    std::visit(
        [&](auto &&c) {
          using T = std::decay_t<decltype(c)>;
          if constexpr (std::is_same_v<T, ActionModel>) {
            auto a = c;
            fixupAction(a);
            m.children.push_back(std::move(a));
          } else if constexpr (std::is_same_v<T, std::shared_ptr<ActionPannelSectionModelWire>>) {
            auto sec = std::make_shared<ActionPannelSectionModel>(toModel(*c));
            m.children.push_back(std::move(sec));
          } else if constexpr (std::is_same_v<T, std::shared_ptr<ActionPannelSubmenuModelWire>>) {
            auto sub = std::make_shared<ActionPannelSubmenuModel>(toModel(*c));
            m.children.push_back(std::move(sub));
          }
        },
        child);
  }
  return m;
}

namespace glz {
template <> struct from<JSON, ActionPannelModel> {
  template <auto Opts, auto... Extra>
  static void op(ActionPannelModel &value, is_context auto &&ctx, auto &&it, auto &&end) {
    ActionPannelModelWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (!bool(ctx.error)) value = toModel(w);
  }
};
} // namespace glz

// ============================================================
// TagListModel wire (children are tag-items → items field)
// ============================================================

struct TagListModelWire {
  std::string title;
  std::vector<TagItemModel> children;
};

template <> struct glz::meta<TagListModelWire> {
  using T = TagListModelWire;
  static constexpr auto value = glz::object("title", &T::title, "children", &T::children);
};

namespace glz {
template <> struct from<JSON, TagListModel> {
  template <auto Opts, auto... Extra>
  static void op(TagListModel &value, is_context auto &&ctx, auto &&it, auto &&end) {
    TagListModelWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value.title = toQStr(w.title);
    value.items = QList<TagItemModel>(w.children.begin(), w.children.end());
  }
};
} // namespace glz

// ============================================================
// MetadataLabel wire (FlexText for text field)
// ============================================================

struct MetadataLabelWire {
  std::string title;
  FlexText text;
  std::optional<ImageURL> icon;
};

template <> struct glz::meta<MetadataLabelWire> {
  using T = MetadataLabelWire;
  static constexpr auto value = glz::object("title", &T::title, "text", &T::text, "icon", &T::icon);
};

namespace glz {
template <> struct from<JSON, MetadataLabel> {
  template <auto Opts, auto... Extra>
  static void op(MetadataLabel &value, is_context auto &&ctx, auto &&it, auto &&end) {
    MetadataLabelWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value.title = toQStr(w.title);
    value.text = toQStr(w.text.value);
    value.icon = std::move(w.icon);
    value.color = std::move(w.text.color);
  }
};
} // namespace glz

// ============================================================
// MetadataModel (heterogeneous children)
// ============================================================

using MetadataChild = std::variant<MetadataLabel, MetadataLink, MetadataSeparator, TagListModel>;

template <> struct glz::meta<MetadataSeparator> {
  static constexpr auto value = glz::object();
};

template <> struct glz::meta<MetadataChild> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids = std::array{"metadata-label", "metadata-link", "metadata-separator", "tag-list"};
};

struct MetadataModelWire {
  std::vector<MetadataChild> children;
};

template <> struct glz::meta<MetadataModelWire> {
  using T = MetadataModelWire;
  static constexpr auto value = glz::object("children", &T::children);
};

namespace glz {
template <> struct from<JSON, MetadataModel> {
  template <auto Opts, auto... Extra>
  static void op(MetadataModel &value, is_context auto &&ctx, auto &&it, auto &&end) {
    MetadataModelWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value.children.reserve(w.children.size());
    for (auto &child : w.children) {
      std::visit([&](auto &&c) { value.children.emplace_back(std::move(c)); }, child);
    }
  }
};
} // namespace glz

// ============================================================
// DetailModel (children: metadata node)
// ============================================================

using DetailChild = std::variant<MetadataModel>;

template <> struct glz::meta<DetailChild> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids = std::array{"metadata"};
};

struct DetailModelWire {
  std::optional<std::string> markdown;
  std::vector<DetailChild> children;
};

template <> struct glz::meta<DetailModelWire> {
  using T = DetailModelWire;
  static constexpr auto value = glz::object("markdown", &T::markdown, "children", &T::children);
};

namespace glz {
template <> struct from<JSON, DetailModel> {
  template <auto Opts, auto... Extra>
  static void op(DetailModel &value, is_context auto &&ctx, auto &&it, auto &&end) {
    DetailModelWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value.markdown = toOptQStr(w.markdown);
    for (auto &child : w.children) {
      if (auto *m = std::get_if<MetadataModel>(&child)) { value.metadata = std::move(*m); }
    }
  }
};
} // namespace glz

// ============================================================
// EmptyViewModel (children: action-panel)
// ============================================================

using SingleAPChild = std::variant<ActionPannelModel>;

template <> struct glz::meta<SingleAPChild> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids = std::array{"action-panel"};
};

struct EmptyViewModelWire {
  std::string title;
  std::string description;
  std::optional<ImageLikeModel> icon;
  std::vector<SingleAPChild> children;
};

template <> struct glz::meta<EmptyViewModelWire> {
  using T = EmptyViewModelWire;
  static constexpr auto value = glz::object("title", &T::title, "description", &T::description, "icon",
                                            &T::icon, "children", &T::children);
};

namespace glz {
template <> struct from<JSON, EmptyViewModel> {
  template <auto Opts, auto... Extra>
  static void op(EmptyViewModel &value, is_context auto &&ctx, auto &&it, auto &&end) {
    EmptyViewModelWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value.title = toQStr(w.title);
    value.description = toQStr(w.description);
    value.icon = std::move(w.icon);
    for (auto &child : w.children) {
      if (auto *ap = std::get_if<ActionPannelModel>(&child)) value.actions = std::move(*ap);
    }
  }
};
} // namespace glz

// ============================================================
// DropdownModel::Section (homogeneous children → items)
// ============================================================

struct DropdownSectionWire {
  std::string title;
  std::vector<DropdownModel::Item> children;
};

template <> struct glz::meta<DropdownSectionWire> {
  using T = DropdownSectionWire;
  static constexpr auto value = glz::object("title", &T::title, "children", &T::children);
};

namespace glz {
template <> struct from<JSON, DropdownModel::Section> {
  template <auto Opts, auto... Extra>
  static void op(DropdownModel::Section &value, is_context auto &&ctx, auto &&it, auto &&end) {
    DropdownSectionWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value.title = toQStr(w.title);
    value.items = std::move(w.children);
  }
};
} // namespace glz

// ============================================================
// DropdownModel (heterogeneous children: item or section)
// ============================================================

using DropdownChild = std::variant<DropdownModel::Item, DropdownModel::Section>;

template <> struct glz::meta<DropdownChild> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids = std::array{"dropdown-item", "dropdown-section"};
};

struct DropdownModelWire {
  std::optional<std::string> tooltip;
  std::optional<std::string> defaultValue;
  std::optional<std::string> id;
  std::optional<std::string> onChange;
  std::optional<std::string> onSearchTextChange;
  std::optional<std::string> placeholder;
  std::optional<std::string> value;
  bool storeValue = true;
  bool throttle = false;
  bool isLoading = false;
  DropdownModel::Filtering filtering = {.keepSectionOrder = true, .enabled = true};
  std::vector<DropdownChild> children;
};

template <> struct glz::meta<DropdownModelWire> {
  using T = DropdownModelWire;
  static constexpr auto value =
      glz::object("tooltip", &T::tooltip, "defaultValue", &T::defaultValue, "id", &T::id, "onChange",
                  &T::onChange, "onSearchTextChange", &T::onSearchTextChange, "placeholder", &T::placeholder,
                  "value", &T::value, "storeValue", &T::storeValue, "throttle", &T::throttle, "isLoading",
                  &T::isLoading, "filtering", &T::filtering, "children", &T::children);
};

namespace glz {
template <> struct from<JSON, DropdownModel> {
  template <auto Opts, auto... Extra>
  static void op(DropdownModel &value, is_context auto &&ctx, auto &&it, auto &&end) {
    DropdownModelWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value.dirty = true;
    value.tooltip = toOptQStr(w.tooltip);
    value.defaultValue = toOptQStr(w.defaultValue);
    value.id = toOptQStr(w.id);
    value.onChange = toOptQStr(w.onChange);
    value.onSearchTextChange = toOptQStr(w.onSearchTextChange);
    value.placeholder = toOptQStr(w.placeholder);
    value.value = toOptQStr(w.value);
    value.storeValue = w.storeValue;
    value.throttle = w.throttle;
    value.isLoading = w.isLoading;
    value.filtering = w.filtering;
    value.children.reserve(w.children.size());
    for (auto &child : w.children) {
      std::visit([&](auto &&c) { value.children.emplace_back(std::move(c)); }, child);
    }
  }
};
} // namespace glz

// ============================================================
// ListItemViewModel wire
// ============================================================

struct ListItemViewModelWire {
  std::string id;
  FlexString title;
  FlexString subtitle;
  std::optional<ImageLikeWrapped> icon;
  std::vector<AccessoryModel> accessories;
  std::vector<std::string> keywords;

  using Child = std::variant<ActionPannelModel, DetailModel>;
  std::vector<Child> children;
};

template <> struct glz::meta<ListItemViewModelWire::Child> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids = std::array{"action-panel", "list-item-detail"};
};

template <> struct glz::meta<ListItemViewModelWire> {
  using T = ListItemViewModelWire;
  static constexpr auto value =
      glz::object("id", &T::id, "title", &T::title, "subtitle", &T::subtitle, "icon", &T::icon, "keywords",
                  &T::keywords, "accessories", &T::accessories, "children", &T::children);
};

namespace glz {
template <> struct from<JSON, ListItemViewModel> {
  template <auto Opts, auto... Extra>
  static void op(ListItemViewModel &value, is_context auto &&ctx, auto &&it, auto &&end) {
    ListItemViewModelWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value.changed = true;
    value.id = std::move(w.id);
    value.title = std::move(w.title.value);
    value.subtitle = std::move(w.subtitle.value);
    value.icon = w.icon ? std::optional(std::move(w.icon->value)) : std::nullopt;
    value.accessories = std::move(w.accessories);
    value.keywords = std::move(w.keywords);
    for (auto &child : w.children) {
      std::visit(
          [&](auto &&c) {
            using T = std::decay_t<decltype(c)>;
            if constexpr (std::is_same_v<T, ActionPannelModel>)
              value.actionPannel = std::move(c);
            else if constexpr (std::is_same_v<T, DetailModel>)
              value.detail = std::move(c);
          },
          child);
    }
  }
};
} // namespace glz

// ============================================================
// ListSectionModel (homogeneous children: list-item)
// ============================================================

struct ListSectionModelWire {
  std::string title;
  std::string subtitle;
  std::vector<ListItemViewModel> children;
};

template <> struct glz::meta<ListSectionModelWire> {
  using T = ListSectionModelWire;
  static constexpr auto value =
      glz::object("title", &T::title, "subtitle", &T::subtitle, "children", &T::children);
};

namespace glz {
template <> struct from<JSON, ListSectionModel> {
  template <auto Opts, auto... Extra>
  static void op(ListSectionModel &value, is_context auto &&ctx, auto &&it, auto &&end) {
    ListSectionModelWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value.title = std::move(w.title);
    value.subtitle = std::move(w.subtitle);
    value.children.reserve(w.children.size());
    size_t index = 0;
    for (auto &item : w.children) {
      if (item.id.empty()) item.id = std::to_string(index);
      value.children.emplace_back(std::move(item));
      ++index;
    }
  }
};
} // namespace glz

// ============================================================
// ListModel (heterogeneous children)
// ============================================================

using ListWireChild =
    std::variant<ListItemViewModel, ListSectionModel, ActionPannelModel, EmptyViewModel, DropdownModel>;

template <> struct glz::meta<ListWireChild> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids =
      std::array{"list-item", "list-section", "action-panel", "empty-view", "dropdown"};
};

struct ListModelWire {
  bool isLoading = false;
  std::optional<bool> filtering;
  bool throttle = false;
  bool isShowingDetail = false;
  std::string navigationTitle;
  std::string searchPlaceholderText;
  std::optional<std::string> onSelectionChanged;
  std::optional<std::string> onSearchTextChange;
  std::optional<EventCounted<std::string>> searchText;
  std::optional<PaginationModel> pagination;
  std::vector<ListWireChild> children;
};

template <> struct glz::meta<ListModelWire> {
  using T = ListModelWire;
  static constexpr auto value =
      glz::object("isLoading", &T::isLoading, "throttle", &T::throttle, "isShowingDetail",
                  &T::isShowingDetail, "navigationTitle", &T::navigationTitle, "searchBarPlaceholder",
                  &T::searchPlaceholderText, "filtering", &T::filtering, "onSearchTextChange",
                  &T::onSearchTextChange, "onSelectionChange", &T::onSelectionChanged, "searchText",
                  &T::searchText, "pagination", &T::pagination, "children", &T::children);
};

static ListModel toListModel(ListModelWire &&w) {
  ListModel m;
  m.dirty = true;
  m.propsDirty = true;
  m.isLoading = w.isLoading;
  m.filtering = w.filtering.value_or(!w.onSearchTextChange.has_value());
  m.throttle = w.throttle;
  m.isShowingDetail = w.isShowingDetail;
  m.navigationTitle = std::move(w.navigationTitle);
  m.searchPlaceholderText = std::move(w.searchPlaceholderText);
  m.onSelectionChanged = std::move(w.onSelectionChanged);
  m.onSearchTextChange = std::move(w.onSearchTextChange);
  m.searchText = std::move(w.searchText);
  m.pagination = std::move(w.pagination);

  m.items.reserve(w.children.size());
  size_t index = 0;
  for (auto &child : w.children) {
    std::visit(
        [&](auto &&c) {
          using T = std::decay_t<decltype(c)>;
          if constexpr (std::is_same_v<T, ListItemViewModel>) {
            if (c.id.empty()) c.id = std::to_string(index);
            m.items.emplace_back(std::move(c));
          } else if constexpr (std::is_same_v<T, ListSectionModel>) {
            m.items.emplace_back(std::move(c));
          } else if constexpr (std::is_same_v<T, ActionPannelModel>) {
            m.actions = std::move(c);
          } else if constexpr (std::is_same_v<T, EmptyViewModel>) {
            m.emptyView = std::move(c);
          } else if constexpr (std::is_same_v<T, DropdownModel>) {
            m.searchBarAccessory = std::move(c);
          }
        },
        child);
    ++index;
  }

  return m;
}

// ============================================================
// GridItemViewModel wire
// ============================================================

struct GridItemViewModelWire {
  std::string id;
  std::string title;
  std::string subtitle;
  GridContentParsed content;
  std::optional<std::string> tooltip;
  std::vector<std::string> keywords;

  std::vector<SingleAPChild> children;
};

template <> struct glz::meta<GridItemViewModelWire> {
  using T = GridItemViewModelWire;
  static constexpr auto value =
      glz::object("id", &T::id, "title", &T::title, "subtitle", &T::subtitle, "content", &T::content,
                  "tooltip", &T::tooltip, "keywords", &T::keywords, "children", &T::children);
};

namespace glz {
template <> struct from<JSON, GridItemViewModel> {
  template <auto Opts, auto... Extra>
  static void op(GridItemViewModel &value, is_context auto &&ctx, auto &&it, auto &&end) {
    GridItemViewModelWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value.id = std::move(w.id);
    value.title = std::move(w.title);
    value.subtitle = std::move(w.subtitle);
    value.content = std::move(w.content.content);
    value.tooltip = w.content.tooltip ? std::move(w.content.tooltip) : std::move(w.tooltip);
    value.keywords = std::move(w.keywords);
    for (auto &child : w.children) {
      if (auto *ap = std::get_if<ActionPannelModel>(&child)) { value.actionPannel = std::move(*ap); }
    }
  }
};
} // namespace glz

// ============================================================
// GridSectionModel (homogeneous children: grid-item)
// ============================================================

struct GridSectionModelWire {
  std::string title;
  std::string subtitle;
  std::optional<double> aspectRatio;
  std::optional<int> columns;
  std::optional<ObjectFit> fit;
  std::optional<GridInset> inset;
  std::vector<GridItemViewModel> children;
};

template <> struct glz::meta<GridSectionModelWire> {
  using T = GridSectionModelWire;
  static constexpr auto value =
      glz::object("title", &T::title, "subtitle", &T::subtitle, "aspectRatio", &T::aspectRatio, "columns",
                  &T::columns, "fit", &T::fit, "inset", &T::inset, "children", &T::children);
};

namespace glz {
template <> struct from<JSON, GridSectionModel> {
  template <auto Opts, auto... Extra>
  static void op(GridSectionModel &value, is_context auto &&ctx, auto &&it, auto &&end) {
    GridSectionModelWire w;
    parse<JSON>::op<Opts>(w, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    value.title = std::move(w.title);
    value.subtitle = std::move(w.subtitle);
    value.aspectRatio = w.aspectRatio;
    value.columns = w.columns;
    value.fit = w.fit;
    value.inset = w.inset;
    value.children.reserve(w.children.size());
    size_t index = 0;
    for (auto &item : w.children) {
      if (item.id.empty()) item.id = std::to_string(index);
      value.children.emplace_back(std::move(item));
      ++index;
    }
  }
};
} // namespace glz

// ============================================================
// GridModel (heterogeneous children)
// ============================================================

using GridWireChild =
    std::variant<GridItemViewModel, GridSectionModel, ActionPannelModel, EmptyViewModel, DropdownModel>;

template <> struct glz::meta<GridWireChild> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids =
      std::array{"grid-item", "grid-section", "action-panel", "empty-view", "dropdown"};
};

struct GridModelWire {
  bool isLoading = false;
  std::optional<bool> filtering;
  bool throttle = false;
  double aspectRatio = 1.0;
  std::optional<int> columns;
  GridInset inset = GridInset::None;
  ObjectFit fit = ObjectFit::Contain;
  std::string navigationTitle;
  std::string searchPlaceholderText;
  std::optional<std::string> onSelectionChanged;
  std::optional<std::string> onSearchTextChange;
  std::optional<std::string> selectedItemId;
  std::optional<EventCounted<std::string>> searchText;
  std::optional<PaginationModel> pagination;
  std::vector<GridWireChild> children;
};

template <> struct glz::meta<GridModelWire> {
  using T = GridModelWire;
  static constexpr auto value =
      glz::object("isLoading", &T::isLoading, "throttle", &T::throttle, "aspectRatio", &T::aspectRatio,
                  "columns", &T::columns, "inset", &T::inset, "fit", &T::fit, "navigationTitle",
                  &T::navigationTitle, "searchBarPlaceholder", &T::searchPlaceholderText, "filtering",
                  &T::filtering, "onSearchTextChange", &T::onSearchTextChange, "onSelectionChange",
                  &T::onSelectionChanged, "selectedItemId", &T::selectedItemId, "searchText", &T::searchText,
                  "pagination", &T::pagination, "children", &T::children);
};

static GridModel toGridModel(GridModelWire &&w) {
  GridModel m;
  m.dirty = true;
  m.isLoading = w.isLoading;
  m.filtering = w.filtering.value_or(!w.onSearchTextChange.has_value());
  m.throttle = w.throttle;
  m.aspectRatio = w.aspectRatio;
  m.columns = w.columns;
  m.inset = w.inset;
  m.fit = w.fit;
  m.navigationTitle = std::move(w.navigationTitle);
  m.searchPlaceholderText = std::move(w.searchPlaceholderText);
  m.onSelectionChanged = std::move(w.onSelectionChanged);
  m.onSearchTextChange = std::move(w.onSearchTextChange);
  m.selectedItemId = std::move(w.selectedItemId);
  m.searchText = std::move(w.searchText);
  m.pagination = std::move(w.pagination);

  m.items.reserve(w.children.size());
  size_t index = 0;
  for (auto &child : w.children) {
    std::visit(
        [&](auto &&c) {
          using T = std::decay_t<decltype(c)>;
          if constexpr (std::is_same_v<T, GridItemViewModel>) {
            if (c.id.empty()) c.id = std::to_string(index);
            m.items.emplace_back(std::move(c));
          } else if constexpr (std::is_same_v<T, GridSectionModel>) {
            m.items.emplace_back(std::move(c));
          } else if constexpr (std::is_same_v<T, ActionPannelModel>) {
            m.actions = std::move(c);
          } else if constexpr (std::is_same_v<T, EmptyViewModel>) {
            m.emptyView = std::move(c);
          } else if constexpr (std::is_same_v<T, DropdownModel>) {
            m.searchBarAccessory = std::move(c);
          }
        },
        child);
    ++index;
  }

  return m;
}

// ============================================================
// RootDetailModel (children: metadata, action-panel)
// ============================================================

using RootDetailChild = std::variant<MetadataModel, ActionPannelModel>;

template <> struct glz::meta<RootDetailChild> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids = std::array{"metadata", "action-panel"};
};

struct RootDetailModelWire {
  bool isLoading = false;
  std::string markdown;
  std::optional<std::string> navigationTitle;
  std::vector<RootDetailChild> children;
};

template <> struct glz::meta<RootDetailModelWire> {
  using T = RootDetailModelWire;
  static constexpr auto value = glz::object("isLoading", &T::isLoading, "markdown", &T::markdown,
                                            "navigationTitle", &T::navigationTitle, "children", &T::children);
};

static RootDetailModel toRootDetailModel(RootDetailModelWire &&w) {
  RootDetailModel m;
  m.isLoading = w.isLoading;
  m.markdown = toQStr(w.markdown);
  m.navigationTitle = toOptQStr(w.navigationTitle);

  for (auto &child : w.children) {
    std::visit(
        [&](auto &&c) {
          using T = std::decay_t<decltype(c)>;
          if constexpr (std::is_same_v<T, MetadataModel>)
            m.metadata = std::move(c);
          else if constexpr (std::is_same_v<T, ActionPannelModel>)
            m.actions = std::move(c);
        },
        child);
  }

  return m;
}

// ============================================================
// Top-level payload (single read, tagged variant for root)
// ============================================================

struct FormModelFallback {
  QJsonObject obj;
};

namespace glz {
template <> struct from<JSON, FormModelFallback> {
  template <auto Opts, auto... Extra>
  static void op(FormModelFallback &value, is_context auto &&ctx, auto &&it, auto &&end) {
    from<JSON, QJsonObject>::op<Opts>(value.obj, ctx, it, end);
  }
};
} // namespace glz

using RootWireModel = std::variant<ListModelWire, GridModelWire, RootDetailModelWire, FormModelFallback>;

template <> struct glz::meta<RootWireModel> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids = std::array{"list", "grid", "detail", "form"};
};

struct ViewEntry {
  bool dirty = true;
  bool propsDirty = true;
  std::optional<RootWireModel> root;
};

template <> struct glz::meta<ViewEntry> {
  using T = ViewEntry;
  static constexpr auto value =
      glz::object("dirty", &T::dirty, "propsDirty", &T::propsDirty, "root", &T::root);
};

struct RenderPayload {
  std::vector<ViewEntry> views;
};

template <> struct glz::meta<RenderPayload> {
  using T = RenderPayload;
  static constexpr auto value = glz::object("views", &T::views);
};

ParsedRenderData parseRenderPayload(std::string_view json) {
  auto t0 = std::chrono::steady_clock::now();

  RenderPayload payload;
  if (auto err = glz::read<PARSE_OPTS>(payload, json)) {
    qWarning() << "parseRenderPayload: failed:" << QString::fromStdString(glz::format_error(err, json));
    return {};
  }

  auto t1 = std::chrono::steady_clock::now();

  ParsedRenderData result;
  result.items.reserve(payload.views.size());

  for (auto &view : payload.views) {
    RenderRoot rr;
    rr.dirty = view.dirty;
    rr.propsDirty = view.propsDirty;

    if (!view.root.has_value()) {
      rr.root = InvalidModel{QString("Empty view root")};
      result.items.emplace_back(std::move(rr));
      continue;
    }

    std::visit(
        [&](auto &&w) {
          using W = std::decay_t<decltype(w)>;
          if constexpr (std::is_same_v<W, ListModelWire>) {
            auto model = toListModel(std::move(w));
            model.dirty = view.dirty;
            model.propsDirty = view.propsDirty;
            rr.root = std::move(model);
          } else if constexpr (std::is_same_v<W, GridModelWire>) {
            auto model = toGridModel(std::move(w));
            model.dirty = view.dirty;
            rr.root = std::move(model);
          } else if constexpr (std::is_same_v<W, RootDetailModelWire>) {
            rr.root = toRootDetailModel(std::move(w));
          } else if constexpr (std::is_same_v<W, FormModelFallback>) {
            rr.root = FormModel::fromJson(w.obj);
          }
        },
        *view.root);

    result.items.emplace_back(std::move(rr));
  }

  auto t2 = std::chrono::steady_clock::now();
  auto parseMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
  auto convertMs = std::chrono::duration<double, std::milli>(t2 - t1).count();
  qWarning().nospace() << "[PERF] parseRenderPayload: glaze=" << parseMs << "ms, convert=" << convertMs
                       << "ms, total=" << (parseMs + convertMs)
                       << "ms, json=" << (json.size() / 1024.0 / 1024.0) << "MB";

  return result;
}

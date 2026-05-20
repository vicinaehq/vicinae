#include "extend/model-deser.hpp"

#include <chrono>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <QColor>
#include <QJsonDocument>
#include <glaze/json.hpp>

#include "common/types.hpp"
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
#include "extend/pagination-model.hpp"
#include "extend/root-detail-model.hpp"
#include "extend/tag-model.hpp"
#include "glaze-qt.hpp"
#include "lib/keyboard/keyboard.hpp"
#include "theme.hpp"
#include "ui/image/url.hpp"
#include "ui/omni-painter/omni-painter.hpp"

static constexpr std::string_view TAG = "$t";
static constexpr glz::opts PARSE_OPTS{.error_on_unknown_keys = false, .minified = true};

static inline QString toQStr(const std::string &s) {
  return QString::fromUtf8(s.data(), static_cast<qsizetype>(s.size()));
}

static inline std::optional<QString> toOptQStr(const std::optional<std::string> &s) {
  return s ? std::optional(toQStr(*s)) : std::nullopt;
}

struct DynColorRaw {
  std::string light;
  std::string dark;
  bool adjustContrast = true;
};

struct ColorLikeWire {
  std::optional<std::string> raw;
  std::optional<DynColorRaw> dynamic;
};

struct ThemedIconSourceWire {
  std::string light;
  std::string dark;
};

struct ImageSourceWire {
  std::optional<std::string> raw;
  std::optional<ThemedIconSourceWire> themed;
  std::optional<std::string> light;
  std::optional<std::string> dark;
};

using ImageSourceVariant = std::variant<std::string, ImageSourceWire>;

struct ImageObjectWire {
  std::optional<ImageSourceVariant> source;
  std::optional<ImageSourceVariant> fallback;
  std::optional<ColorLikeWire> tintColor;
  std::optional<std::string> mask;
  std::optional<std::string> fileIcon;
};

using ImageLikeWire = std::variant<std::string, ImageObjectWire>;

struct FlexTextObj {
  std::string value;
  std::optional<ColorLikeWire> color;
};

using FlexText = std::variant<std::string, FlexTextObj>;

struct FlexStringObj {
  std::string value;
};

using FlexString = std::variant<std::string, FlexStringObj>;

struct AccessoryWire {
  std::optional<ImageLikeWire> icon;
  std::optional<std::string> tooltip;
  std::optional<FlexText> tag;
  std::optional<FlexText> text;
};

struct GridContentObjWire {
  std::optional<std::string> tooltip;
  std::optional<ColorLikeWire> color;
  std::optional<ImageLikeWire> value;
  std::optional<ImageSourceVariant> source;
  std::optional<ImageSourceVariant> fallback;
  std::optional<ColorLikeWire> tintColor;
  std::optional<std::string> mask;
  std::optional<std::string> fileIcon;
};

using GridContentWire = std::variant<std::string, GridContentObjWire>;

struct ImageWrappedObjWire {
  std::optional<ImageLikeWire> value;
  std::optional<ImageSourceVariant> source;
  std::optional<ImageSourceVariant> fallback;
  std::optional<ColorLikeWire> tintColor;
  std::optional<std::string> mask;
  std::optional<std::string> fileIcon;
};

using ImageWrappedWire = std::variant<std::string, ImageWrappedObjWire>;

template <> struct glz::meta<ObjectFit> {
  using enum ObjectFit;
  static constexpr auto value = glz::enumerate("contain", Contain, "fill", Fill, "stretch", Stretch);
};

template <> struct glz::meta<GridInset> {
  using enum GridInset;
  static constexpr auto value =
      glz::enumerate("zero", None, "small", Small, "medium", Medium, "large", Large);
};

template <> struct glz::meta<OmniPainter::ImageMaskType> {
  using enum OmniPainter::ImageMaskType;
  static constexpr auto value =
      glz::enumerate("circle", CircleMask, "roundedRectangle", RoundedRectangleMask);
};

struct ShortcutObjWire {
  std::string key;
  std::vector<std::string> modifiers;
};

using ShortcutWire = std::variant<std::string, ShortcutObjWire>;

struct ActionModelWire {
  std::string title;
  std::string onAction;
  std::optional<std::string> onSubmit;
  std::optional<ImageLikeWire> icon;
  std::optional<ShortcutWire> shortcut;
  std::string type = "callback";
  glz::raw_json quicklink;
  std::optional<std::string> stableId;
};

struct ActionPannelSectionModelWire {
  std::string title;
  std::vector<glz::raw_json> children;
};

struct ActionPannelSubmenuModelWire {
  std::string title;
  std::optional<ImageLikeWire> icon;
  std::optional<ShortcutWire> shortcut;
  std::optional<bool> autoFocus;
  std::optional<std::variant<bool, ActionPannelSubmenuFiltering>> filtering;
  std::optional<bool> isLoading;
  std::optional<bool> throttle;
  std::string onOpen;
  std::string onSearchTextChange;
  std::vector<glz::raw_json> children;
  std::optional<std::string> stableId;
};

using ActionPannelItemWire =
    std::variant<ActionModelWire, ActionPannelSectionModelWire, ActionPannelSubmenuModelWire>;

template <> struct glz::meta<ActionPannelItemWire> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids = std::array{"action", "action-panel-section", "action-panel-submenu"};
};

struct ActionPannelModelWire {
  std::string title;
  std::vector<ActionPannelItemWire> children;
  std::optional<std::string> stableId;
};

struct DropdownItemWire {
  std::string title;
  std::string value;
  std::optional<ImageLikeWire> icon;
  std::vector<std::string> keywords;
};

struct DropdownSectionWire {
  std::string title;
  std::vector<DropdownItemWire> children;
};

using DropdownChildWire = std::variant<DropdownItemWire, DropdownSectionWire>;

template <> struct glz::meta<DropdownChildWire> {
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
  std::vector<DropdownChildWire> children;
  bool storeValue = true;
  bool throttle = false;
  std::optional<bool> filtering;
  bool isLoading = false;
};

struct TagItemWire {
  std::string text;
  std::string onAction;
  std::optional<ImageLikeWire> icon;
  std::optional<ColorLikeWire> color;
};

struct TagListWire {
  std::string title;
  std::vector<TagItemWire> children;
};

struct MetadataLabelWire {
  std::string title;
  FlexText text;
  std::optional<ImageLikeWire> icon;
};

template <> struct glz::meta<MetadataSeparator> {
  static constexpr auto value = glz::object();
};

using MetadataChild = std::variant<MetadataLabelWire, MetadataLink, MetadataSeparator, TagListWire>;

template <> struct glz::meta<MetadataChild> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids = std::array{"metadata-label", "metadata-link", "metadata-separator", "tag-list"};
};

struct MetadataModelWire {
  std::vector<MetadataChild> children;
};

using DetailChild = std::variant<MetadataModelWire>;

template <> struct glz::meta<DetailChild> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids = std::array{"metadata"};
};

struct DetailModelWire {
  std::optional<std::string> markdown;
  std::vector<DetailChild> children;
};

using SingleAPChild = std::variant<ActionPannelModelWire>;

template <> struct glz::meta<SingleAPChild> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids = std::array{"action-panel"};
};

struct EmptyViewModelWire {
  std::string title;
  std::string description;
  std::optional<ImageLikeWire> icon;
  std::vector<SingleAPChild> children;
};

using ListItemChild = std::variant<ActionPannelModelWire, DetailModelWire>;

template <> struct glz::meta<ListItemChild> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids = std::array{"action-panel", "list-item-detail"};
};

struct ListItemViewModelWire {
  std::string id;
  FlexString title;
  FlexString subtitle;
  std::optional<ImageWrappedWire> icon;
  std::vector<AccessoryWire> accessories;
  std::vector<std::string> keywords;
  std::vector<ListItemChild> children;
};

struct ListSectionModelWire {
  std::string title;
  std::string subtitle;
  std::vector<ListItemViewModelWire> children;
};

using ListWireChild = std::variant<ListItemViewModelWire, ListSectionModelWire, ActionPannelModelWire,
                                   EmptyViewModelWire, DropdownModelWire>;

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
  std::string searchBarPlaceholder;
  std::optional<std::string> onSelectionChange;
  std::optional<std::string> onSearchTextChange;
  std::optional<EventCounted<std::string>> searchText;
  std::optional<PaginationModel> pagination;
  std::vector<ListWireChild> children;
};

struct GridItemViewModelWire {
  std::string id;
  std::string title;
  std::string subtitle;
  GridContentWire content;
  std::optional<std::string> tooltip;
  std::vector<std::string> keywords;
  std::vector<SingleAPChild> children;
};

struct GridSectionModelWire {
  std::string title;
  std::string subtitle;
  std::optional<double> aspectRatio;
  std::optional<int> columns;
  std::optional<ObjectFit> fit;
  std::optional<GridInset> inset;
  std::vector<GridItemViewModelWire> children;
};

using GridWireChild = std::variant<GridItemViewModelWire, GridSectionModelWire, ActionPannelModelWire,
                                   EmptyViewModelWire, DropdownModelWire>;

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
  std::string searchBarPlaceholder;
  std::optional<std::string> onSelectionChange;
  std::optional<std::string> onSearchTextChange;
  std::optional<std::string> selectedItemId;
  std::optional<EventCounted<std::string>> searchText;
  std::optional<PaginationModel> pagination;
  std::vector<GridWireChild> children;
};

using RootDetailChild = std::variant<MetadataModelWire, ActionPannelModelWire>;

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

static std::string takeString(FlexString v) {
  return match(
      v, [](std::string &s) { return std::move(s); }, [](FlexStringObj &o) { return std::move(o.value); });
}

static ColorLike toColorLike(ColorLikeWire w) {
  if (w.raw) {
    auto qs = toQStr(*w.raw);
    if (auto tint = ImageURL::tintForName(qs); tint != SemanticColor::InvalidTint) return tint;
    return qs;
  }
  if (w.dynamic) {
    return DynamicColor{.light = toQStr(w.dynamic->light),
                        .dark = toQStr(w.dynamic->dark),
                        .adjustContrast = w.dynamic->adjustContrast};
  }
  return {};
}

static std::optional<ColorLike> toOptColorLike(std::optional<ColorLikeWire> w) {
  return w ? std::optional(toColorLike(std::move(*w))) : std::nullopt;
}

static std::variant<QString, ThemedIconSource> toImageSource(ImageSourceVariant v) {
  return match(
      v, [](std::string &s) -> std::variant<QString, ThemedIconSource> { return toQStr(s); },
      [](ImageSourceWire &w) -> std::variant<QString, ThemedIconSource> {
        if (w.raw) return toQStr(*w.raw);
        if (w.themed)
          return ThemedIconSource{.light = toQStr(w.themed->light), .dark = toQStr(w.themed->dark)};
        if (w.light)
          return ThemedIconSource{.light = toQStr(*w.light), .dark = w.dark ? toQStr(*w.dark) : QString()};
        return QString();
      });
}

static ImageLikeModel toImageLike(ImageLikeWire v) {
  return match(
      v, [](std::string &s) -> ImageLikeModel { return ExtensionImageModel{.source = toQStr(s)}; },
      [](ImageObjectWire &w) -> ImageLikeModel {
        if (w.source) {
          ExtensionImageModel model;
          model.source = toImageSource(std::move(*w.source));
          if (w.fallback) model.fallback = toImageSource(std::move(*w.fallback));
          if (w.tintColor) model.tintColor = toColorLike(std::move(*w.tintColor));
          if (w.mask) model.mask = OmniPainter::maskForName(toQStr(*w.mask));
          return model;
        }
        if (w.fileIcon) return ExtensionFileIconModel{.file = *w.fileIcon};
        return InvalidImageModel{};
      });
}

static std::optional<ImageLikeModel> toOptImageLike(std::optional<ImageLikeWire> w) {
  return w ? std::optional(toImageLike(std::move(*w))) : std::nullopt;
}

static ImageURL toImageURL(ImageLikeWire v) { return ImageURL(toImageLike(std::move(v))); }

static std::optional<ImageURL> toOptImageURL(std::optional<ImageLikeWire> w) {
  return w ? std::optional(toImageURL(std::move(*w))) : std::nullopt;
}

static ImageLikeModel toImageWrapped(ImageWrappedWire v) {
  return match(
      v, [](std::string &s) -> ImageLikeModel { return ExtensionImageModel{.source = toQStr(s)}; },
      [](ImageWrappedObjWire &w) -> ImageLikeModel {
        if (w.value && !w.source && !w.fileIcon) return toImageLike(std::move(*w.value));
        if (w.source) {
          ExtensionImageModel model;
          model.source = toImageSource(std::move(*w.source));
          if (w.fallback) model.fallback = toImageSource(std::move(*w.fallback));
          if (w.tintColor) model.tintColor = toColorLike(std::move(*w.tintColor));
          if (w.mask) model.mask = OmniPainter::maskForName(toQStr(*w.mask));
          return model;
        }
        if (w.fileIcon) return ExtensionFileIconModel{.file = *w.fileIcon};
        return InvalidImageModel{};
      });
}

static GridItemViewModel::Content toGridContent(GridContentWire v, std::optional<std::string> &tooltip) {
  return match(
      v,
      [](std::string &s) -> GridItemViewModel::Content { return ExtensionImageModel{.source = toQStr(s)}; },
      [&tooltip](GridContentObjWire &w) -> GridItemViewModel::Content {
        if (w.tooltip) tooltip = std::move(*w.tooltip);
        if (w.color) return toColorLike(std::move(*w.color));
        if (w.value) return toImageLike(std::move(*w.value));
        if (w.source) {
          ExtensionImageModel model;
          model.source = toImageSource(std::move(*w.source));
          if (w.fallback) model.fallback = toImageSource(std::move(*w.fallback));
          if (w.tintColor) model.tintColor = toColorLike(std::move(*w.tintColor));
          if (w.mask) model.mask = OmniPainter::maskForName(toQStr(*w.mask));
          return model;
        }
        if (w.fileIcon) return ExtensionFileIconModel{.file = *w.fileIcon};
        return InvalidImageModel{};
      });
}

static AccessoryModel toAccessoryModel(AccessoryWire w) {
  AccessoryModel m;
  m.icon = toOptImageLike(std::move(w.icon));
  if (w.tooltip) m.tooltip = std::move(*w.tooltip);

  auto unpackFlexText = [](FlexText &ft) -> std::pair<std::string, std::optional<ColorLike>> {
    return match(
        ft,
        [](std::string &s) -> std::pair<std::string, std::optional<ColorLike>> {
          return {std::move(s), std::nullopt};
        },
        [](FlexTextObj &o) -> std::pair<std::string, std::optional<ColorLike>> {
          return {std::move(o.value), toOptColorLike(std::move(o.color))};
        });
  };

  if (w.tag) {
    auto [val, color] = unpackFlexText(*w.tag);
    m.data = AccessoryModel::Tag{.color = std::move(color), .value = std::move(val)};
  } else if (w.text) {
    auto [val, color] = unpackFlexText(*w.text);
    m.data = AccessoryModel::Text{.color = std::move(color), .value = std::move(val)};
  }
  return m;
}

static const std::unordered_map<std::string, Keybind> NAMED_SHORTCUTS = {
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

static Keyboard::Shortcut toShortcut(ShortcutWire w) {
  return match(
      w,
      [](std::string &s) -> Keyboard::Shortcut {
        if (auto found = NAMED_SHORTCUTS.find(s); found != NAMED_SHORTCUTS.end()) return found->second;
        return Keyboard::Shortcut::fromString(toQStr(s));
      },
      [](ShortcutObjWire &o) -> Keyboard::Shortcut {
        auto k = Keyboard::keyFromString(toQStr(o.key));
        if (!k) return {};
        Qt::KeyboardModifiers mods{};
        for (const auto &m : o.modifiers) {
          auto mod = Keyboard::modifierFromString(toQStr(m));
          if (mod) mods.setFlag(*mod);
        }
        return Keyboard::Shortcut(*k, mods);
      });
}

static std::optional<Keyboard::Shortcut> toOptShortcut(std::optional<ShortcutWire> w) {
  return w ? std::optional(toShortcut(std::move(*w))) : std::nullopt;
}

static ActionModel toActionModel(ActionModelWire w) {
  ActionModel m;
  m.title = std::move(w.title);
  m.onAction = std::move(w.onAction);
  m.onSubmit = std::move(w.onSubmit);
  m.icon = toOptImageLike(std::move(w.icon));
  m.shortcut = toOptShortcut(std::move(w.shortcut));
  m.type = std::move(w.type);
  if (!w.quicklink.str.empty()) {
    auto doc =
        QJsonDocument::fromJson(QByteArray::fromRawData(w.quicklink.str.data(), w.quicklink.str.size()));
    m.quicklink = doc.object();
  }
  m.stableId = std::move(w.stableId);
  return m;
}

static ActionPannelSectionModel toActionPannelSection(ActionPannelSectionModelWire w);
static ActionPannelSubmenuModel toActionPannelSubmenu(ActionPannelSubmenuModelWire w);

static ActionPannelSectionModel toActionPannelSection(ActionPannelSectionModelWire w) {
  ActionPannelSectionModel m;
  m.title = std::move(w.title);
  m.items.reserve(w.children.size());
  for (auto &raw : w.children) {
    ActionPannelItemWire child{};
    if (auto err = glz::read<PARSE_OPTS>(child, raw.str)) continue;
    match(
        std::move(child), [&](ActionModelWire c) { m.items.emplace_back(toActionModel(std::move(c))); },
        [&](ActionPannelSubmenuModelWire c) {
          m.items.emplace_back(
              std::make_shared<ActionPannelSubmenuModel>(toActionPannelSubmenu(std::move(c))));
        },
        [&](ActionPannelSectionModelWire) {});
  }
  return m;
}

static ActionPannelSubmenuModel toActionPannelSubmenu(ActionPannelSubmenuModelWire w) {
  ActionPannelSubmenuModel m;
  m.title = std::move(w.title);
  m.icon = toOptImageLike(std::move(w.icon));
  m.shortcut = toOptShortcut(std::move(w.shortcut));
  m.autoFocus = w.autoFocus;
  m.filtering = std::move(w.filtering);
  m.isLoading = w.isLoading;
  m.throttle = w.throttle;
  m.onOpen = std::move(w.onOpen);
  m.onSearchTextChange = std::move(w.onSearchTextChange);
  m.stableId = std::move(w.stableId);
  m.children.reserve(w.children.size());
  for (auto &raw : w.children) {
    ActionPannelItemWire child{};
    if (auto err = glz::read<PARSE_OPTS>(child, raw.str)) continue;
    match(
        std::move(child),
        [&](ActionPannelSectionModelWire c) {
          m.children.emplace_back(
              std::make_shared<ActionPannelSectionModel>(toActionPannelSection(std::move(c))));
        },
        [&](ActionModelWire c) { m.children.emplace_back(toActionModel(std::move(c))); },
        [&](ActionPannelSubmenuModelWire c) {
          m.children.emplace_back(
              std::make_shared<ActionPannelSubmenuModel>(toActionPannelSubmenu(std::move(c))));
        });
  }
  return m;
}

static ActionPannelModel toActionPannelModel(ActionPannelModelWire w) {
  ActionPannelModel m;
  m.title = std::move(w.title);
  m.stableId = std::move(w.stableId);
  m.children.reserve(w.children.size());
  for (auto &child : w.children) {
    match(
        std::move(child), [&](ActionModelWire c) { m.children.emplace_back(toActionModel(std::move(c))); },
        [&](ActionPannelSectionModelWire c) {
          m.children.emplace_back(
              std::make_shared<ActionPannelSectionModel>(toActionPannelSection(std::move(c))));
        },
        [&](ActionPannelSubmenuModelWire c) {
          m.children.emplace_back(
              std::make_shared<ActionPannelSubmenuModel>(toActionPannelSubmenu(std::move(c))));
        });
  }
  return m;
}

static DropdownModel::Item toDropdownItem(DropdownItemWire w) {
  return {.title = std::move(w.title),
          .value = std::move(w.value),
          .icon = toOptImageLike(std::move(w.icon)),
          .keywords = std::move(w.keywords)};
}

static DropdownModel::Section toDropdownSection(DropdownSectionWire w) {
  DropdownModel::Section m;
  m.title = std::move(w.title);
  m.items.reserve(w.children.size());
  for (auto &c : w.children)
    m.items.emplace_back(toDropdownItem(std::move(c)));
  return m;
}

static DropdownModel toDropdownModel(DropdownModelWire w) {
  DropdownModel m;
  m.tooltip = std::move(w.tooltip);
  m.defaultValue = std::move(w.defaultValue);
  m.id = std::move(w.id);
  m.onChange = std::move(w.onChange);
  m.onSearchTextChange = std::move(w.onSearchTextChange);
  m.placeholder = std::move(w.placeholder);
  m.value = std::move(w.value);
  m.storeValue = w.storeValue;
  m.throttle = w.throttle;
  m.filtering.enabled = w.filtering.value_or(true);
  m.isLoading = w.isLoading;
  m.children.reserve(w.children.size());
  for (auto &child : w.children) {
    match(
        child, [&](DropdownItemWire &c) { m.children.emplace_back(toDropdownItem(std::move(c))); },
        [&](DropdownSectionWire &c) { m.children.emplace_back(toDropdownSection(std::move(c))); });
  }
  return m;
}

static TagItemModel toTagItem(TagItemWire w) {
  return {.text = std::move(w.text),
          .icon = toOptImageLike(std::move(w.icon)),
          .color = toOptColorLike(std::move(w.color)),
          .onAction = std::move(w.onAction)};
}

static TagListModel toTagList(TagListWire w) {
  TagListModel m;
  m.title = std::move(w.title);
  m.items.reserve(w.children.size());
  for (auto &c : w.children)
    m.items.emplace_back(toTagItem(std::move(c)));
  return m;
}

static MetadataLabel toMetadataLabel(MetadataLabelWire w) {
  MetadataLabel m;
  m.title = std::move(w.title);
  m.icon = toOptImageURL(std::move(w.icon));
  match(
      w.text, [&](std::string &s) { m.text = std::move(s); },
      [&](FlexTextObj &o) {
        m.text = std::move(o.value);
        m.color = toOptColorLike(std::move(o.color));
      });
  return m;
}

static MetadataModel toMetadataModel(MetadataModelWire w) {
  MetadataModel m;
  m.children.reserve(w.children.size());
  for (auto &child : w.children) {
    match(
        child, [&](MetadataLabelWire &c) { m.children.emplace_back(toMetadataLabel(std::move(c))); },
        [&](TagListWire &c) { m.children.emplace_back(toTagList(std::move(c))); },
        [&](auto &c) { m.children.emplace_back(std::move(c)); });
  }
  return m;
}

static DetailModel toDetailModel(DetailModelWire w) {
  DetailModel m;
  m.markdown = std::move(w.markdown);
  for (auto &child : w.children) {
    if (auto *mw = std::get_if<MetadataModelWire>(&child)) m.metadata = toMetadataModel(std::move(*mw));
  }
  return m;
}

static EmptyViewModel toEmptyViewModel(EmptyViewModelWire w) {
  EmptyViewModel m;
  m.title = std::move(w.title);
  m.description = std::move(w.description);
  m.icon = toOptImageLike(std::move(w.icon));
  for (auto &child : w.children) {
    if (auto *ap = std::get_if<ActionPannelModelWire>(&child))
      m.actions = toActionPannelModel(std::move(*ap));
  }
  return m;
}

static ListItemViewModel toListItemViewModel(ListItemViewModelWire w) {
  ListItemViewModel m;
  m.changed = true;
  m.id = std::move(w.id);
  m.title = takeString(std::move(w.title));
  m.subtitle = takeString(std::move(w.subtitle));
  m.icon = w.icon ? std::optional(toImageWrapped(std::move(*w.icon))) : std::nullopt;
  m.accessories.reserve(w.accessories.size());
  for (auto &a : w.accessories)
    m.accessories.emplace_back(toAccessoryModel(std::move(a)));
  m.keywords = std::move(w.keywords);
  for (auto &child : w.children) {
    match(
        child, [&](ActionPannelModelWire &c) { m.actionPannel = toActionPannelModel(std::move(c)); },
        [&](DetailModelWire &c) { m.detail = toDetailModel(std::move(c)); });
  }
  return m;
}

static ListSectionModel toListSectionModel(ListSectionModelWire w) {
  ListSectionModel m;
  m.title = std::move(w.title);
  m.subtitle = std::move(w.subtitle);
  m.children.reserve(w.children.size());
  for (size_t i = 0; i < w.children.size(); ++i) {
    auto item = toListItemViewModel(std::move(w.children[i]));
    if (item.id.empty()) item.id = std::to_string(i);
    m.children.emplace_back(std::move(item));
  }
  return m;
}

static ListModel toListModel(ListModelWire w) {
  ListModel m;
  m.dirty = true;
  m.isLoading = w.isLoading;
  m.filtering = w.filtering.value_or(!w.onSearchTextChange.has_value());
  m.throttle = w.throttle;
  m.isShowingDetail = w.isShowingDetail;
  m.navigationTitle = std::move(w.navigationTitle);
  m.searchPlaceholderText = std::move(w.searchBarPlaceholder);
  m.onSelectionChanged = std::move(w.onSelectionChange);
  m.onSearchTextChange = std::move(w.onSearchTextChange);
  m.searchText = std::move(w.searchText);
  m.pagination = std::move(w.pagination);

  m.items.reserve(w.children.size());
  size_t index = 0;
  for (auto &child : w.children) {
    match(
        child,
        [&](ListItemViewModelWire &c) {
          auto item = toListItemViewModel(std::move(c));
          if (item.id.empty()) item.id = std::to_string(index);
          m.items.emplace_back(std::move(item));
        },
        [&](ListSectionModelWire &c) { m.items.emplace_back(toListSectionModel(std::move(c))); },
        [&](ActionPannelModelWire &c) { m.actions = toActionPannelModel(std::move(c)); },
        [&](EmptyViewModelWire &c) { m.emptyView = toEmptyViewModel(std::move(c)); },
        [&](DropdownModelWire &c) { m.searchBarAccessory = toDropdownModel(std::move(c)); });
    ++index;
  }

  return m;
}

static GridItemViewModel toGridItemViewModel(GridItemViewModelWire w) {
  GridItemViewModel m;
  m.id = std::move(w.id);
  m.title = std::move(w.title);
  m.subtitle = std::move(w.subtitle);
  m.tooltip = std::move(w.tooltip);
  m.content = toGridContent(std::move(w.content), m.tooltip);
  m.keywords = std::move(w.keywords);
  for (auto &child : w.children) {
    if (auto *ap = std::get_if<ActionPannelModelWire>(&child))
      m.actionPannel = toActionPannelModel(std::move(*ap));
  }
  return m;
}

static GridSectionModel toGridSectionModel(GridSectionModelWire w) {
  GridSectionModel m;
  m.title = std::move(w.title);
  m.subtitle = std::move(w.subtitle);
  m.aspectRatio = w.aspectRatio;
  m.columns = w.columns;
  m.fit = w.fit;
  m.inset = w.inset;
  m.children.reserve(w.children.size());
  for (size_t i = 0; i < w.children.size(); ++i) {
    auto item = toGridItemViewModel(std::move(w.children[i]));
    if (item.id.empty()) item.id = std::to_string(i);
    m.children.emplace_back(std::move(item));
  }
  return m;
}

static GridModel toGridModel(GridModelWire w) {
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
  m.searchPlaceholderText = std::move(w.searchBarPlaceholder);
  m.onSelectionChanged = std::move(w.onSelectionChange);
  m.onSearchTextChange = std::move(w.onSearchTextChange);
  m.selectedItemId = std::move(w.selectedItemId);
  m.searchText = std::move(w.searchText);
  m.pagination = std::move(w.pagination);

  m.items.reserve(w.children.size());
  size_t index = 0;
  for (auto &child : w.children) {
    match(
        child,
        [&](GridItemViewModelWire &c) {
          auto item = toGridItemViewModel(std::move(c));
          if (item.id.empty()) item.id = std::to_string(index);
          m.items.emplace_back(std::move(item));
        },
        [&](GridSectionModelWire &c) { m.items.emplace_back(toGridSectionModel(std::move(c))); },
        [&](ActionPannelModelWire &c) { m.actions = toActionPannelModel(std::move(c)); },
        [&](EmptyViewModelWire &c) { m.emptyView = toEmptyViewModel(std::move(c)); },
        [&](DropdownModelWire &c) { m.searchBarAccessory = toDropdownModel(std::move(c)); });
    ++index;
  }

  return m;
}

static RootDetailModel toRootDetailModel(RootDetailModelWire w) {
  RootDetailModel m;
  m.isLoading = w.isLoading;
  m.markdown = std::move(w.markdown);
  m.navigationTitle = std::move(w.navigationTitle);

  for (auto &child : w.children) {
    match(
        child, [&](MetadataModelWire &c) { m.metadata = toMetadataModel(std::move(c)); },
        [&](ActionPannelModelWire &c) { m.actions = toActionPannelModel(std::move(c)); });
  }

  return m;
}

struct FormFieldValue {
  std::optional<EventCounted<QJsonValue>> val;
};

namespace glz {
template <> struct from<JSON, FormFieldValue> {
  template <auto Opts, auto... Extra>
  static void op(FormFieldValue &value, is_context auto &&ctx, auto &&it, auto &&end) {
    generic g;
    parse<JSON>::op<Opts>(g, ctx, it, end);
    if (bool(ctx.error)) [[unlikely]]
      return;
    if (g.is_object()) {
      auto &obj = g.get_object();
      if (auto ecIt = obj.find("eventCount"); ecIt != obj.end() && ecIt->second.is_number()) {
        QJsonValue inner;
        if (auto vIt = obj.find("value"); vIt != obj.end()) inner = glazeToQJsonValue(vIt->second);
        value.val = EventCounted<QJsonValue>{inner, static_cast<uint32_t>(ecIt->second.get_number())};
        return;
      }
    }
    value.val = EventCounted<QJsonValue>{glazeToQJsonValue(g), EVENT_COUNT_UNTRACKED};
  }
};
} // namespace glz

// clang-format off
#define FIELD_BASE_WIRE_MEMBERS                     \
  std::string id;                                   \
  bool autoFocus = false;                           \
  std::optional<std::string> error;                 \
  std::optional<std::string> info;                  \
  std::optional<std::string> onBlur;                \
  std::optional<std::string> onChange;               \
  std::optional<std::string> onFocus;               \
  std::optional<std::string> title;                 \
  bool storeValue = true;                           \
  FormFieldValue value;                             \
  std::optional<glz::generic> defaultValue;

// clang-format on

template <typename T> static FormModel::FieldBase toFieldBase(T &w) {
  FormModel::FieldBase b;
  b.id = std::move(w.id);
  b.autoFocus = w.autoFocus;
  b.error = std::move(w.error);
  b.info = std::move(w.info);
  b.onBlur = std::move(w.onBlur);
  b.onChange = std::move(w.onChange);
  b.onFocus = std::move(w.onFocus);
  b.title = std::move(w.title);
  b.storeValue = w.storeValue;
  b.value = std::move(w.value.val);
  if (w.defaultValue) b.defaultValue = glazeToQJsonValue(*w.defaultValue);
  return b;
}

struct FormTextFieldWire {
  FIELD_BASE_WIRE_MEMBERS
  std::optional<std::string> placeholder;
};

struct FormPasswordFieldWire {
  FIELD_BASE_WIRE_MEMBERS
  std::optional<std::string> placeholder;
};

struct FormCheckboxFieldWire {
  FIELD_BASE_WIRE_MEMBERS
  std::optional<std::string> label;
};

struct FormDropdownFieldWire {
  FIELD_BASE_WIRE_MEMBERS
  std::optional<std::string> onSearchTextChange;
  std::optional<std::string> placeholder;
  std::optional<std::string> tooltip;
  bool isLoading = false;
  bool throttle = false;
  std::optional<bool> filtering;
  std::vector<DropdownChildWire> children;
};

struct FormTextAreaFieldWire {
  FIELD_BASE_WIRE_MEMBERS
  std::optional<std::string> placeholder;
};

struct FormFilePickerFieldWire {
  FIELD_BASE_WIRE_MEMBERS
  bool allowMultipleSelection = false;
  bool canChooseDirectories = false;
  bool canChooseFiles = true;
  bool showHiddenFiles = false;
};

struct FormDatePickerFieldWire {
  FIELD_BASE_WIRE_MEMBERS
  std::optional<std::string> min;
  std::optional<std::string> max;
  std::optional<std::string> type;
};

struct FormSeparatorWire {};

struct FormDescriptionWire {
  std::string text;
  std::optional<std::string> title;
};

struct FormLinkAccessoryWire {
  std::string text;
  std::string target;
};

struct FormIgnoredWire {};

using FormChild =
    std::variant<FormTextFieldWire, FormPasswordFieldWire, FormCheckboxFieldWire, FormDropdownFieldWire,
                 FormTextAreaFieldWire, FormFilePickerFieldWire, FormDatePickerFieldWire, FormSeparatorWire,
                 FormDescriptionWire, FormLinkAccessoryWire, ActionPannelModelWire, FormIgnoredWire>;

template <> struct glz::meta<FormChild> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids =
      std::array{"text-field",       "password-field",    "checkbox-field",    "dropdown-field",
                 "text-area-field",  "file-picker-field", "date-picker-field", "separator",
                 "form-description", "link-accessory",    "action-panel"};
};

struct FormModelWire {
  bool isLoading = false;
  bool enableDrafts = false;
  std::optional<std::string> navigationTitle;
  std::vector<FormChild> children;
};

static FormModel toFormModel(FormModelWire w) {
  FormModel m;
  m.isLoading = w.isLoading;
  m.enableDrafts = w.enableDrafts;
  m.navigationTitle = std::move(w.navigationTitle);
  m.items.reserve(w.children.size());

  for (auto &child : w.children) {
    match(
        child,
        [&](FormTextFieldWire &c) {
          m.items.emplace_back(
              FormModel::Field{FormModel::TextField{toFieldBase(c), std::move(c.placeholder)}});
        },
        [&](FormPasswordFieldWire &c) {
          m.items.emplace_back(
              FormModel::Field{FormModel::PasswordField{toFieldBase(c), std::move(c.placeholder)}});
        },
        [&](FormCheckboxFieldWire &c) {
          m.items.emplace_back(
              FormModel::Field{FormModel::CheckboxField{toFieldBase(c), std::move(c.label)}});
        },
        [&](FormDropdownFieldWire &c) {
          FormModel::DropdownField f;
          f.base = toFieldBase(c);
          f.onSearchTextChange = std::move(c.onSearchTextChange);
          f.placeholder = std::move(c.placeholder);
          f.tooltip = std::move(c.tooltip);
          f.isLoading = c.isLoading;
          f.throttle = c.throttle;
          f.filtering = c.filtering.value_or(!f.onSearchTextChange.has_value());
          f.items.reserve(c.children.size());
          for (auto &dc : c.children) {
            match(
                dc, [&](DropdownItemWire &v) { f.items.emplace_back(toDropdownItem(std::move(v))); },
                [&](DropdownSectionWire &v) { f.items.emplace_back(toDropdownSection(std::move(v))); });
          }
          m.items.emplace_back(FormModel::Field{std::move(f)});
        },
        [&](FormTextAreaFieldWire &c) {
          m.items.emplace_back(
              FormModel::Field{FormModel::TextAreaField{toFieldBase(c), std::move(c.placeholder)}});
        },
        [&](FormFilePickerFieldWire &c) {
          FormModel::FilePickerField f;
          f.base = toFieldBase(c);
          f.allowMultipleSelection = c.allowMultipleSelection;
          f.canChooseDirectories = c.canChooseDirectories;
          f.canChooseFiles = c.canChooseFiles;
          f.showHiddenFiles = c.showHiddenFiles;
          m.items.emplace_back(FormModel::Field{std::move(f)});
        },
        [&](FormDatePickerFieldWire &c) {
          FormModel::DatePickerField f;
          f.base = toFieldBase(c);
          f.min = std::move(c.min);
          f.max = std::move(c.max);
          f.type = std::move(c.type);
          m.items.emplace_back(FormModel::Field{std::move(f)});
        },
        [&](FormSeparatorWire &) { m.items.emplace_back(FormModel::Separator{}); },
        [&](FormDescriptionWire &c) {
          FormModel::Description desc;
          desc.text = std::move(c.text);
          desc.title = std::move(c.title);
          m.items.emplace_back(std::move(desc));
        },
        [&](FormLinkAccessoryWire &c) {
          FormModel::LinkAccessoryModel link;
          link.text = std::move(c.text);
          link.target = std::move(c.target);
          m.searchBarAccessory = std::move(link);
        },
        [&](ActionPannelModelWire &c) { m.actions = toActionPannelModel(std::move(c)); },
        [&](FormIgnoredWire &) {});
  }

  return m;
}

using RootWireModel = std::variant<ListModelWire, GridModelWire, RootDetailModelWire, FormModelWire>;

template <> struct glz::meta<RootWireModel> {
  static constexpr std::string_view tag = TAG;
  static constexpr auto ids = std::array{"list", "grid", "detail", "form"};
};

struct ViewEntry {
  bool dirty = true;
  std::optional<RootWireModel> root;
};

struct RenderPayload {
  std::vector<ViewEntry> views;
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

    if (!view.root.has_value()) {
      rr.root = InvalidModel{QString("Empty view root")};
      result.items.emplace_back(std::move(rr));
      continue;
    }

    match(
        *view.root,
        [&](ListModelWire &w) {
          auto model = toListModel(std::move(w));
          model.dirty = view.dirty;
          rr.root = std::move(model);
        },
        [&](GridModelWire &w) {
          auto model = toGridModel(std::move(w));
          model.dirty = view.dirty;
          rr.root = std::move(model);
        },
        [&](RootDetailModelWire &w) { rr.root = toRootDetailModel(std::move(w)); },
        [&](FormModelWire &w) { rr.root = toFormModel(std::move(w)); });

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

#pragma once
#include "services/root-item-manager/root-item-manager.hpp"

// Official Segoe Fluent Icons glyph names (all also present in Segoe MDL2 Assets on Windows 10).
enum class SegoeIcon : char16_t {
  Add = 0xE710,
  Airplane = 0xE709,
  AllApps = 0xE71D,
  Battery10 = 0xE83F,
  Bluetooth = 0xE702,
  CC = 0xE7F0,
  Camera = 0xE722,
  CellPhone = 0xE8EA,
  Cloud = 0xE753,
  Color = 0xE790,
  Contact = 0xE77B,
  DeveloperTools = 0xEC7A,
  Devices = 0xE772,
  Diagnostic = 0xE9D9,
  DockBottom = 0xE90E,
  DownloadMap = 0xE826,
  Edit = 0xE70F,
  Equalizer = 0xE9E9,
  Ethernet = 0xE839,
  FavoriteStar = 0xE734,
  Fingerprint = 0xE928,
  Font = 0xE8D2,
  FontSize = 0xE8E9,
  Game = 0xE7FC,
  Globe = 0xE774,
  GridView = 0xF0E2,
  HardDrive = 0xEDA2,
  Headphone = 0xE7F6,
  History = 0xE81C,
  Info = 0xE946,
  InternetSharing = 0xE704,
  KeyboardClassic = 0xE765,
  Lightbulb = 0xEA80,
  LightningBolt = 0xE945,
  Link = 0xE71B,
  Location = 0xE81D,
  Lock = 0xE72E,
  Mail = 0xE715,
  MapPin = 0xE707,
  Microphone = 0xE720,
  MiracastLogoSmall = 0xEC15,
  Mouse = 0xE962,
  Narrator = 0xED4D,
  NetworkAdapter = 0xEDA3,
  OpenWith = 0xE7AC,
  Paste = 0xE77F,
  People = 0xE716,
  Permissions = 0xE8D7,
  Personalize = 0xE771,
  Phone = 0xE717,
  Picture = 0xE8B9,
  Play = 0xE768,
  Print = 0xE749,
  QuietHours = 0xE708,
  Recent = 0xE823,
  RedEye = 0xE7B3,
  Remote = 0xE8AF,
  Repair = 0xE90F,
  Ringer = 0xEA8F,
  RingerSilent = 0xE7ED,
  Search = 0xE721,
  Settings = 0xE713,
  Share = 0xE72D,
  Shield = 0xEA18,
  SpeedHigh = 0xEC4A,
  Sync = 0xE895,
  TVMonitor = 0xE7F4,
  TaskView = 0xE7C4,
  TimeLanguage = 0xE775,
  Touchpad = 0xEFA5,
  USB = 0xE88E,
  UpdateRestore = 0xE777,
  VPN = 0xE705,
  Video = 0xE714,
  Volume = 0xE767,
  Wifi = 0xE701,
  Work = 0xE821,
  Zoom = 0xE71E,
};

struct WinSettingsPage {
  const char *id;
  const char *title;
  const char *category;
  const char *url;
  SegoeIcon glyph;
  const char *keywords;
};

class WinSettingsPageRootItem : public RootItem {
  const WinSettingsPage &m_page;

  QString title() const override;
  QString subtitle() const override;
  QString typeDisplayName() const override;
  ImageURL iconUrl() const override;
  EntrypointId uniqueId() const override;
  AccessoryList accessories() const override;
  std::vector<QString> keywords() const override;
  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                   const RootItemMetadata &metadata) const override;
  std::vector<std::pair<QString, QString>> settingsMetadata() const override;

public:
  explicit WinSettingsPageRootItem(const WinSettingsPage &page) : m_page(page) {}
};

class WinSettingsRootProvider : public RootProvider {
public:
  QString uniqueId() const override;
  QString displayName() const override;
  QString description() const override;
  ImageURL icon() const override;
  Type type() const override;
  std::vector<std::shared_ptr<RootItem>> loadItems() const override;
};

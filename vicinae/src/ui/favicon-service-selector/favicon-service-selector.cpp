#include "favicon-service-selector.hpp"
#include "favicon/favicon-service.hpp"

class FaviconServiceItem : public SelectorInput::AbstractItem {
  FaviconService::FaviconServiceData m_service;

  QString displayName() const override { return m_service.name; }

  QString generateId() const override { return m_service.id; }

  std::optional<ImageURL> icon() const override { return m_service.icon; }

  AbstractItem *clone() const override { return new FaviconServiceItem(*this); }

public:
  const FaviconService::FaviconServiceData &service() const { return m_service; }

  FaviconServiceItem(const FaviconService::FaviconServiceData &data) : m_service(data) {}
};

FaviconServiceSelector::FaviconServiceSelector() {
  auto items = FaviconService::providers() |
               std::views::transform([](auto &&theme) -> std::shared_ptr<SelectorInput::AbstractItem> {
                 return std::make_shared<FaviconServiceItem>(theme);
               }) |
               std::ranges::to<std::vector>();

  addSection("", items);
  updateModel();
}

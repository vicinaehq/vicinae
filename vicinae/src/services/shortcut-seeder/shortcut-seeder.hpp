#include "services/app-service/app-service.hpp"
#include "services/shortcut/shortcut-service.hpp"

class ShortcutSeeder {
public:
  ShortcutSeeder(ShortcutService &shortcut, const AppService &appDb);

  void seed();

private:
  ShortcutService &m_service;
  const AppService &m_appDb;
};

#include "ui/image/image.hpp"
#include <qfuturewatcher.h>

class StaticIODeviceImageLoader : public AbstractImageLoader {
  using ImageWatcher = QFutureWatcher<QImage>;
  QSharedPointer<ImageWatcher> m_watcher;
  std::unique_ptr<QIODevice> m_device = nullptr;
  bool m_started = false;

  static QImage loadStatic(std::unique_ptr<QIODevice> device, const RenderConfig &cfg);

public:
  void abort() const override;
  void render(const RenderConfig &cfg) override;

public:
  StaticIODeviceImageLoader(std::unique_ptr<QIODevice> device);
  ~StaticIODeviceImageLoader();
};

#include "ui/image/image.hpp"
#include <qbuffer.h>
#include <qfuturewatcher.h>
#include <qstringview.h>

class StaticIODeviceImageLoader : public AbstractImageLoader {
  using ImageWatcher = QFutureWatcher<QImage>;
  QSharedPointer<ImageWatcher> m_watcher;
  QByteArray m_data;

  static QImage loadStatic(const QByteArray &data, const RenderConfig &cfg);

public:
  void abort() const override;
  void render(const RenderConfig &cfg) override;

public:
  StaticIODeviceImageLoader(const QByteArray &data);
  ~StaticIODeviceImageLoader();
};

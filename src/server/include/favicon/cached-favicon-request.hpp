#include <QPixmap>
#include "favicon/favicon-request.hpp"

class CachedFaviconRequest : public AbstractFaviconRequest {
  QPixmap _data;

  void start() override;

public:
  CachedFaviconRequest(const QString &domain, const QPixmap &data, QObject *parent = nullptr);
};

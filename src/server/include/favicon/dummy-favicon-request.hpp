#include "favicon/favicon-request.hpp"
#include <QDebug>

class DummyFaviconRequest : public AbstractFaviconRequest {
public:
  DummyFaviconRequest(const QString &domain, QObject *parent = nullptr);

  void start() override;
};

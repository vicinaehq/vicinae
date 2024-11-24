#include <QPainter>
#include <QWidget>

class ColorCircle : public QWidget {
  QString s;
  QSize size;

protected:
  void paintEvent(QPaintEvent *event) override;

  QSize sizeHint() const override;

public:
  ColorCircle(const QString &color, QSize size, QWidget *parent = nullptr);
};

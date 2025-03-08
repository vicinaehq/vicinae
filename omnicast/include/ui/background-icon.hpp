#include <qpainter.h>
#include <qwidget.h>

class BackgroundIcon : public QWidget {
  QString _name;
  QColor _bg;
  QColor _fg;
  int _radius;

  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);

    painter.setBrush(QBrush(_bg));
    painter.setPen(_bg);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawRoundedRect(rect(), _radius, _radius);

    QPixmap icon;

    QPixmap pixmap(_name);
  }

public:
  void setIconName(const QString &name) { _name = name; }

  BackgroundIcon() {}
};

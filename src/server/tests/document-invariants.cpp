#include <QAbstractItemModelTester>
#include <QImage>
#include <QPersistentModelIndex>
#include <QSignalSpy>
#include <QTest>
#include <QTextBoundaryFinder>
#include <QTextDocument>
#include "builtins/ai/chat-conversation-model.hpp"
#include "ui/quick/document-text-images.hpp"

class DocumentInvariantsTest : public QObject {
  Q_OBJECT

private slots:
  void conversationPreservesModelIndexes() {
    ChatConversationModel model;
    QAbstractItemModelTester tester(&model, QAbstractItemModelTester::FailureReportingMode::QtTest);
    QSignalSpy resets(&model, &QAbstractItemModel::modelReset);
    QSignalSpy removals(&model, &QAbstractItemModel::rowsRemoved);
    model.beginExchange("Question");
    const QPersistentModelIndex first(model.index(0));
    model.appendResponse("Answer");
    model.finishExchange();
    model.beginExchange("Follow-up");
    model.appendResponse("Partial response");
    model.finishExchange("Connection interrupted");
    QVERIFY(first.isValid());
    QCOMPARE(first.data(ChatConversationModel::ResponseRole).toString(), "Answer");
    QCOMPARE(model.rowCount(), 2);
    QCOMPARE(model.index(1).data(ChatConversationModel::ResponseRole).toString(), "Partial response");
    QCOMPARE(model.index(1).data(ChatConversationModel::ErrorRole).toString(), "Connection interrupted");
    QVERIFY(!model.index(1).data(ChatConversationModel::PendingRole).toBool());
    QVERIFY(resets.isEmpty());
    QVERIFY(removals.isEmpty());
  }

  void everyPublishedPrefixEndsAtAGraphemeBoundary() {
    ChatConversationModel model;
    model.beginExchange("Question");
    const auto response = QString::fromUtf8("Café 👩🏽‍🌾 🇫🇷 日本語 🌱 ").repeated(20);
    QStringList errors;
    connect(&model, &QAbstractItemModel::dataChanged, &model, [&] {
      const auto visible = model.index(0).data(ChatConversationModel::ResponseRole).toString();
      QTextBoundaryFinder boundary(QTextBoundaryFinder::Grapheme, response);
      boundary.setPosition(visible.size());
      if (!response.startsWith(visible) || !boundary.isAtBoundary()) errors.append(visible);
    });
    model.appendResponse(response.toStdString());
    model.finishExchange();
    QTRY_COMPARE_WITH_TIMEOUT(model.index(0).data(ChatConversationModel::ResponseRole).toString(), response,
                              5000);
    QVERIFY(errors.isEmpty());
  }

  void imageDimensionsSurviveDecodedCacheEviction() {
    DocumentImageCache cache(nullptr);
    const QUrl first("https://example.invalid/field-visit-0.png");
    QImage photograph(2048, 1536, QImage::Format_RGB32);
    photograph.fill(QColor("#467d91"));
    cache.remember(first, photograph);
    for (int image = 1; image < 12; ++image)
      cache.remember(QUrl(QString("https://example.invalid/field-visit-%1.png").arg(image)), photograph);
    QVERIFY(cache.image(first).isNull());
    QCOMPARE(cache.size(first), std::optional<QSize>(photograph.size()));
    QTextDocument document;
    DocumentTextImages::measure(&document, &cache);
    document.setHtml(QString("<img src=\"%1\" width=\"320\">").arg(first.toString()));
    document.setTextWidth(640);
    QVERIFY(document.size().height() >= 240);
    QVERIFY(document.size().height() < 270);
  }
};

QTEST_MAIN(DocumentInvariantsTest)
#include "document-invariants.moc"

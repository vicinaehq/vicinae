#include <QAbstractItemModelTester>
#include <QImage>
#include <QPersistentModelIndex>
#include <QSignalSpy>
#include <QTest>
#include <QTextBoundaryFinder>
#include <QTextDocument>
#include <QTextImageFormat>
#include <QTemporaryDir>
#include <glaze/json.hpp>
#include "services/ai/acp/acp-content.hpp"
#include "services/ai/acp/acp-config.hpp"
#include "builtins/ai/chat-conversation-model.hpp"
#include "ui/quick/document-match-geometry.hpp"
#include "ui/quick/document-text-images.hpp"
#include "ui/quick/math-renderer.hpp"

class DocumentInvariantsTest : public QObject {
  Q_OBJECT

private slots:
  void resourcesPreserveBytesAndHaveVisibleFallbacks() {
    AI::acp::ContentBlock text{
        .type = "resource",
        .resource = AI::acp::ResourceContent{.text = "A saved note", .uri = "file:///tmp/note.txt"}};
    const auto note = AI::acp::resourceContent(text);
    QCOMPARE(note.data.value(), QByteArray("A saved note"));
    QCOMPARE(note.name.value(), "note.txt");
    QCOMPARE(note.mimeType.value(), "text/plain");
    const auto bytes = QByteArray::fromHex("0001020300ff");
    AI::acp::ContentBlock file{.type = "resource",
                               .resource = AI::acp::ResourceContent{.mimeType = "application/pdf",
                                                                    .blob = bytes.toBase64().toStdString()},
                               .name = "../report.pdf"};
    const auto report = AI::acp::resourceContent(file);
    QCOMPARE(report.data.value(), bytes);
    QCOMPARE(report.name.value(), "report.pdf");
    AI::acp::ToolMedia media;
    const std::vector<AI::acp::ToolContent> contents{{.type = "content", .content = file}};
    media.replace(contents);
    QCOMPARE(media.takeNew().size(), std::size_t(1));
    media.replace(contents);
    QVERIFY(media.takeNew().empty());
    const auto remote =
        AI::acp::resourceContent({.type = "resource_link", .uri = "https://example.com/file.pdf"});
    QVERIFY(!remote.data);
    QCOMPARE(remote.status, AI::ResourcePart::Status::Available);
    const auto unknown = AI::acp::resourceContent({.type = "future_content"});
    QCOMPARE(unknown.status, AI::ResourcePart::Status::Unsupported);
    const auto invalid = AI::acp::resourceContent({.type = "audio", .data = "%%%", .mimeType = "audio/wav"});
    QCOMPARE(invalid.status, AI::ResourcePart::Status::Unavailable);
  }

  void reasoningChoicesFollowAgentConfiguration() {
    std::vector<AI::acp::ConfigOption> options{
        {.id = "mode", .category = "mode", .options = std::vector<AI::acp::ConfigEntry>{{.value = "code"}}},
        {.id = "effort",
         .category = "thought_level",
         .type = "select",
         .currentValue = glz::raw_json{"\"high\""},
         .options = std::vector<AI::acp::ConfigEntry>{
             {.value = "low", .name = "Low"},
             {.options = std::vector<AI::acp::ConfigChoice>{{.value = "high", .name = "High"},
                                                            {.value = "xhigh", .name = "Extra high"}}}}}};
    const auto reasoning = AI::acp::reasoningOptions(options);
    QVERIFY(reasoning);
    QCOMPARE(reasoning->id, "effort");
    QCOMPARE(reasoning->choices.size(), std::size_t(3));
    QCOMPARE(reasoning->current.value(), "high");
    QCOMPARE(reasoning->choices.back().id, "xhigh");
    options.back().type = "boolean";
    QVERIFY(!AI::acp::reasoningOptions(options));
    options.pop_back();
    QVERIFY(!AI::acp::reasoningOptions(options));
  }

  void attachmentsStayInTranscriptOrder() {
    ChatConversationModel model;
    QAbstractItemModelTester tester(&model, QAbstractItemModelTester::FailureReportingMode::QtTest);
    model.beginExchange("Create a file");
    model.appendResponse("Before");
    model.appendAttachment({{"name", "report.pdf"}});
    model.appendAttachment({{"name", "summary.txt"}});
    model.addTool({.id = 1, .name = "Read", .status = "completed"});
    model.appendResponse("After");
    model.finishExchange();
    const auto &contents = model.contents(0);
    QCOMPARE(contents.size(), std::size_t(4));
    const auto &attachments = std::get<ChatConversationModel::Attachment>(contents[1]).items;
    QCOMPARE(attachments.size(), 2);
    QCOMPARE(attachments[0].toMap().value("name").toString(), "report.pdf");
    QCOMPARE(attachments[1].toMap().value("name").toString(), "summary.txt");
    model.toggleTool(1);
    QVERIFY(std::get<ChatConversationModel::ToolGroup>(contents[2]).calls.front().expanded);
  }

  void imagesKeepTheirPositionBetweenTextAndTools() {
    ChatConversationModel model;
    QAbstractItemModelTester tester(&model, QAbstractItemModelTester::FailureReportingMode::QtTest);
    model.beginExchange("Draw an image");
    model.appendResponse("Before");
    model.appendImage("image://first");
    model.addTool({.id = 1, .name = "Image generation", .status = "succeeded"});
    model.appendImage("image://second");
    model.appendResponse("After");
    model.finishExchange();
    QTRY_COMPARE(model.index(0).data(ChatConversationModel::ResponseRole).toString(), "BeforeAfter");
    const auto &contents = model.contents(0);
    QCOMPARE(contents.size(), std::size_t(5));
    QCOMPARE(std::get<ChatConversationModel::Response>(contents[0]).visibleBytes, std::size_t(6));
    QCOMPARE(std::get<ChatConversationModel::Image>(contents[1]).source, "image://first");
    QVERIFY(std::holds_alternative<ChatConversationModel::ToolGroup>(contents[2]));
    QCOMPARE(std::get<ChatConversationModel::Image>(contents[3]).source, "image://second");
    QCOMPARE(std::get<ChatConversationModel::Response>(contents[4]).text, "After");
    model.toggleTool(1);
    QVERIFY(std::get<ChatConversationModel::ToolGroup>(contents[2]).calls.front().expanded);
  }

  void acpImagesSurviveRepeatedToolUpdatesWithoutTruncation() {
    const auto bytes = QByteArray(24000, 'x');
    AI::acp::ContentBlock block;
    const auto json = std::string("{\"type\":\"image\",\"data\":\"") + bytes.toBase64().toStdString() +
                      "\",\"mimeType\":\"image/png\",\"uri\":\"file:///image.png\"}";
    QVERIFY(!glz::read<glz::opts{.error_on_unknown_keys = false}>(block, json));
    const std::vector<AI::acp::ToolContent> contents{
        {.type = "content", .content = AI::acp::ContentBlock{.type = "text", .text = "Image generated"}},
        {.type = "content", .content = block}};
    AI::acp::ToolMedia images;
    images.replace(contents);
    images.replace(contents);
    const auto first = images.takeNew();
    QCOMPARE(first.size(), std::size_t(1));
    QCOMPARE(QByteArray::fromBase64(std::get<AI::ImagePart>(first.front()).base64), bytes);
    images.replace(contents);
    QVERIFY(images.takeNew().empty());
    auto second = contents;
    second.back().content->data = QByteArray("second image").toBase64().toStdString();
    images.replace(second);
    QCOMPARE(images.takeNew().size(), std::size_t(1));
    images.replace(contents);
    images.replace({});
    QVERIFY(images.takeNew().empty());
  }

  void acpImageContentValidatesTypesAndEmbeddedResources() {
    AI::acp::ContentBlock block{.type = "image", .data = "%%%", .mimeType = "image/png"};
    QVERIFY(!AI::acp::imageContent(block));
    block.data = QByteArray("image bytes").toBase64().toStdString();
    block.mimeType = "text/html";
    QVERIFY(!AI::acp::imageContent(block));
    block.type = "resource";
    block.resource = AI::acp::ResourceContent{.mimeType = "image/jpeg", .blob = block.data};
    const auto image = AI::acp::imageContent(block);
    QVERIFY(image);
    QCOMPARE(image->mimeType, "image/jpeg");
    QCOMPARE(QByteArray::fromBase64(image->base64), "image bytes");
  }

  void acpLocalImageReferencesResolveWithoutReadingOtherResources() {
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const auto path = directory.filePath("picture.png");
    QImage picture(2, 2, QImage::Format_ARGB32);
    picture.fill(Qt::red);
    QVERIFY(picture.save(path));
    AI::acp::ContentBlock block{.type = "resource_link", .uri = path.toStdString()};
    const auto image = AI::acp::imageContent(block);
    QVERIFY(image);
    QCOMPARE(image->mimeType, "image/png");
    QCOMPARE(QImage::fromData(QByteArray::fromBase64(image->base64)), picture);
    block.uri = QUrl::fromLocalFile(path).toString().toStdString();
    QVERIFY(AI::acp::imageContent(block));
    block.uri = "https://example.com/picture.png";
    QVERIFY(!AI::acp::imageContent(block));
    block.uri = "picture.png";
    QVERIFY(!AI::acp::imageContent(block));
    QFile text(directory.filePath("plain.png"));
    QVERIFY(text.open(QIODevice::WriteOnly));
    text.write("This is not an image");
    text.close();
    block.uri = text.fileName().toStdString();
    QVERIFY(!AI::acp::imageContent(block));
  }

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

  void matchRectsFollowTheLinesTheMatchSpans() {
    QTextDocument document;
    document.setPlainText(QStringLiteral("first line\nsecond line"));
    document.setTextWidth(1000);

    const auto single = document_match::rects(document, 6, 4);
    QCOMPARE(single.size(), std::size_t(1));
    QVERIFY(single[0].x() > 0);
    QVERIFY(single[0].width() > 0);

    const auto spanning = document_match::rects(document, 6, 10);
    QCOMPARE(spanning.size(), std::size_t(2));
    QVERIFY(spanning[1].y() > spanning[0].y());
    QCOMPARE(spanning[0].x(), single[0].x());
  }

  void mathIsRegisteredAsAVisibleImageAtTheDevicePixelRatio() {
    const auto math = math::render(QStringLiteral("e^{i\\pi} + 1 = 0"), true, Qt::white);
    QVERIFY(math);

    QTextDocument document;
    DocumentTextImages handler;
    handler.setDevicePixelRatio(2);
    QTextImageFormat format;
    format.setName(math->url);
    const auto size = handler.intrinsicSize(&document, 0, format);

    const auto image = document.resource(QTextDocument::ImageResource, QUrl(math->url)).value<QImage>();
    QVERIFY(!image.isNull());
    QCOMPARE(image.devicePixelRatio(), 2.0);
    QCOMPARE(image.size(), (size * 2).toSize());

    bool hasInk = false;
    for (int y = 0; y < image.height() && !hasInk; ++y) {
      for (int x = 0; x < image.width() && !hasInk; ++x) {
        hasInk = qAlpha(image.pixel(x, y)) > 0;
      }
    }
    QVERIFY(hasInk);
  }
};

QTEST_MAIN(DocumentInvariantsTest)
#include "document-invariants.moc"

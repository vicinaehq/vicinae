#pragma once
#include <iostream>
#include <qdatetime.h>
#include <qlogging.h>
#include <filesystem>

void coloredMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

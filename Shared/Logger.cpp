#include "Logger.h"
#include <QtDebug>
#include <QSet>


GlobalLogger globalLogger;

void Logger::setLoggerTag(const QString& tag)
{
	mLoggerTag = tag;
}

void Logger::logDebug(const QString& text)
{
	globalLogger.logSlot(mLoggerTag, "Debug", text);
}

void Logger::logInfo(const QString& text)
{
	globalLogger.logSlot(mLoggerTag, "Info", text);
}

void Logger::logWarning(const QString& text)
{
	globalLogger.logSlot(mLoggerTag, "Warning", text);
}


void GlobalLogger::logSlot(const QString& tag, const QString& level, const QString& text)
{
	QSet<QString> debugTags;
	debugTags.insert("Main");

	if (level=="Debug" and !debugTags.contains(tag)) return;

	qDebug() << QString("%1 %2  |  %3").arg(tag, 8).arg(level, 7).arg(text).toStdString().c_str();
	emit logSignal(tag, level, text);
}

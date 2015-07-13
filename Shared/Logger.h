#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

class Logger
{
protected:
	void setLoggerTag (const QString& tag);

	void logDebug (const QString& text);
	void logInfo (const QString& text);
	void logWarning (const QString& text);

private:
	QString mLoggerTag;
};


class GlobalLogger : public QObject
{
public:
	Q_OBJECT

public slots:
	void logSlot (const QString& tag, const QString& level, const QString& text);

signals:
	void logSignal (const QString& tag, const QString& level, const QString& text);
};

extern GlobalLogger globalLogger;

#endif // LOGGER_H

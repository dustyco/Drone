#ifndef CONFIG_H
#define CONFIG_H

#include <QDebug>
#include <QSettings>
#include <QDir>
#include "../Shared/Logger.h"

class Config : public Logger
{
public:
    Config ()
    {
		setLoggerTag("Config");

        QString path = QDir::homePath() + "/DroneConfig.ini";
		mSettings = new QSettings(path, QSettings::IniFormat);
		//std::cout << "Loading config from: " << mSettings->fileName().toStdString() << std::endl;
		logInfo(QString("Loading config from %1").arg(mSettings->fileName()));
    }
    ~Config ()
    {
        delete mSettings;
    }

    QVariant value ( const QString & key, const QVariant & defaultValue = QVariant() ) const
    {
        return mSettings->value(prefix + key, defaultValue);
    }

    static Config& getSingleton ()
    {
        // Guaranteed to be destroyed.
        // Instantiated on first use.
        static Config instance;
        return instance;
    }

    QString prefix;

private:
    QSettings* mSettings;
};

#endif // CONFIG_H


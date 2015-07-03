#ifndef CONFIG_H
#define CONFIG_H

#include <QDebug>
#include <QSettings>
#include <QDir>

class Config
{
public:
    Config ()
    {
        QString path = QDir::homePath() + "/DroneConfig.ini";
		mSettings = new QSettings(path, QSettings::IniFormat);
		//std::cout << "Loading config from: " << mSettings->fileName().toStdString() << std::endl;
		qDebug() << "Loading config from" << mSettings->fileName();
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


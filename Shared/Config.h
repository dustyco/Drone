#ifndef CONFIG_H
#define CONFIG_H

/*
    [Control]
    Port: 93303
    Tower Host: google.com
    Tower Port: 93304

    [Flight]
    Port: 93302
    Tower Host: google.com
    Tower Port: 93304
    Turning Radius: 30 ft


    [Tower]
    Port: 93304
*/

#include <iostream>
#include <QSettings>
#include <QDir>

class Config
{
public:
    Config ()
    {
        QString path = QDir::homePath() + "/DroneConfig.ini";
        mSettings = new QSettings(path, QSettings::IniFormat);
        std::cout << "Loading config from: " << mSettings->fileName().toStdString() << std::endl;
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


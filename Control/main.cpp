#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "DroneControl.h"
#include <QtQml>

int main(int argc, char *argv[])
{

	qmlRegisterType<DroneControl>("Drone", 1, 0, "DroneControl");

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/main.qml"));

    return app.exec();
}

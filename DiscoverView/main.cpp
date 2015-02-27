#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "DroneDiscoverView.h"
#include <QtQml>

int main(int argc, char *argv[])
{

    qmlRegisterType<DroneDiscoverView>("Drone", 1, 0, "DroneDiscoverView");

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/main.qml"));

    return app.exec();
}

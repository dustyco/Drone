
#include <QCoreApplication>
#include "DroneFlight.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    DroneFlight droneFlight;

    return app.exec();
}

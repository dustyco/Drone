#ifndef DRONETOWER_H
#define DRONETOWER_H

#include <QObject>
#include "../Shared/Discover.h"

class DroneTower : public QObject
{
		Q_OBJECT

public:
		explicit DroneTower (QObject *parent = 0);

signals:

public slots:

private:
		Discover* mDiscover;
};

#endif // DRONETOWER_H

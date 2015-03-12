#ifndef DRONEDISCOVERVIEW_H
#define DRONEDISCOVERVIEW_H

#include <QObject>
#include <QMap>
#include <QTimer>
#include <QUdpSocket>
#include <iostream>
#include <../Shared/Discover.h>
#include <../Shared/Config.h>

class DroneDiscoverView : public QObject
{
    Q_OBJECT
	Q_PROPERTY(QString version READ version CONSTANT)
	Q_PROPERTY(QString text READ text NOTIFY textChanged)

public:
	explicit DroneDiscoverView (QObject *parent = 0);
	QString version () const;
	QString text () const;

signals:
	void textChanged (QString);

public slots:
	void recordFound (Record record);
	void recordLost (Record record);

private:
	Discover* discover;
	QMap<Record,int> records;
};

#endif // DRONEDISCOVERVIEW_H

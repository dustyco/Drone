#ifndef RECORD_H
#define RECORD_H

#include <QMap>
#include <QList>
#include <QString>
#include <QByteArray>
#include <QJsonObject>


/*
 * Reserved keys:
 *
 * _* (Meaning anything starting with _)
 * Scope    _Sc
 * Address  _Ad
 * Port     _Po
 * Instance _In
 * Hostname _Ho
 * Service  _Se
 *
 * */

class Record
{
public:
	bool isEmpty () const;
	bool has (const QString& key) const;
	QString get (const QString& key) const;
	void remove (const QString& key);
	QList<QString> keys () const;
	QString toString () const;
	
	QString operator[] (const QString& key) const;
	QString& operator[] (const QString& key);
	QString operator[] (const char* key) const;
	QString& operator[] (const char* key);
	bool operator < (const Record& right) const;
	bool operator == (const Record& right) const;

public:
	static QList<Record> listFromBytes (const QByteArray& bytes);
	static QByteArray listToBytes (const QList<Record>& record);
	static void compressReserved (Record& record);
	static void decompressReserved (Record& record);

private:
	static Record fromJson (const QJsonObject &json);
	static QJsonObject toJson (Record record);

private:
	QMap<QString,QString> data;
};

#endif // RECORD_H

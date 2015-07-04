#include "Record.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QStringList>

bool Record::isEmpty () const
{
	return data.empty();
}
bool Record::has (const QString& key) const
{
	return data.contains(key);
}
QString Record::get (const QString& key) const
{
	if (data.contains(key)) return data[key];
	else return QString();
}
void Record::remove (const QString& key)
{
	data.remove(key);
}
QList<QString> Record::keys () const
{
	return data.keys();
}

QString Record::operator[] (const QString& key) const
{
	return data[key];
}
QString& Record::operator[] (const QString& key)
{
	return data[key];
}
QString Record::operator[] (const char* key) const
{
	return data[key];
}
QString& Record::operator[] (const char* key)
{
	return data[key];
}
bool Record::operator < (const Record& right) const
{
	if (data.size() != right.data.size())
		return (data.size() < right.data.size());
	else return (toString() < right.toString());
}
bool Record::operator == (const Record& right) const
{
	if (data.size() != right.data.size())
		return false;
	else return (toString() == right.toString());
}

QList<Record> Record::listFromBytes (const QByteArray& bytes)
{
	QList<Record> records;
	QJsonDocument jdoc = QJsonDocument::fromJson(bytes);
	for (auto value : jdoc.array())
	{
		Record record = Record::fromJson(value.toObject());
		if (not record.isEmpty()) records.push_back(record);
	}
	return records;
}

QByteArray Record::listToBytes (const QList<Record>& records)
{
	QJsonArray jsonArray;
	for (Record record : records)
	{
		jsonArray.push_back(Record::toJson(record));
	}
	return QJsonDocument(jsonArray).toJson(QJsonDocument::Compact);
}
/*
* Scope    _Sc
* Address  _Ad
* Port     _Po
* Instance _In
* Hostname _Ho
* Service  _Se
* Machine  _Ma
* Channel  _Ch
*/
void Record::compressReserved (Record& record)
{
	if (record.has("Scope")) { record["_Sc"] = record["Scope"]; record.remove("Scope"); }
	if (record.has("Address")) { record["_Ad"] = record["Address"]; record.remove("Address"); }
	if (record.has("Port")) { record["_Po"] = record["Port"]; record.remove("Port"); }
	if (record.has("Instance")) { record["_In"] = record["Instance"]; record.remove("Instance"); }
	if (record.has("Hostname")) { record["_Ho"] = record["Hostname"]; record.remove("Hostname"); }
	if (record.has("Service")) { record["_Se"] = record["Service"]; record.remove("Service"); }
	if (record.has("Machine")) { record["_Ma"] = record["Machine"]; record.remove("Machine"); }
	if (record.has("Channel")) { record["_Ch"] = record["Channel"]; record.remove("Channel"); }
}

void Record::decompressReserved (Record& record)
{
	if (record.has("_Sc")) { record["Scope"] = record["_Sc"]; record.remove("_Sc"); }
	if (record.has("_Ad")) { record["Address"] = record["_Ad"]; record.remove("_Ad"); }
	if (record.has("_Po")) { record["Port"] = record["_Po"]; record.remove("_Po"); }
	if (record.has("_In")) { record["Instance"] = record["_In"]; record.remove("_In"); }
	if (record.has("_Ho")) { record["Hostname"] = record["_Ho"]; record.remove("_Ho"); }
	if (record.has("_Se")) { record["Service"] = record["_Se"]; record.remove("_Se"); }
	if (record.has("_Ma")) { record["Machine"] = record["_Ma"]; record.remove("_Ma"); }
	if (record.has("_Ch")) { record["Channel"] = record["_Ch"]; record.remove("_Ch"); }
}

QString Record::toString () const
{
	QString ret;
	QString prefix;
	for (QString key : data.keys())
	{
		ret += prefix + key + " " + data[key];
		prefix = " | ";
	}
	return ret;
}

Record Record::fromJson (const QJsonObject& json)
{
	Record record;
	for (QString key : json.keys())
	{
		if (json[key].isString()) record[key] = json[key].toString();
	}
	return record;
}

QJsonObject Record::toJson (Record record)
{
	QJsonObject json;
	for (QString key : record.keys())
	{
		json[key] = record[key];
	}
	return json;
}

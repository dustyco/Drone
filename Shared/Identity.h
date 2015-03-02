#ifndef IDENTITY_H
#define IDENTITY_H

#include <QString>

class Identity
{
private:
	Identity ();

public:
	static QString basedOnInterfaces (QByteArray salt = "");
};

#endif // IDENTITY_H

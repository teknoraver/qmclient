#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>

class Client
{
public:
	Client(QString host);

	QStringList getLines(QString cmd);
	void getLeaves(QString path);
	void browseDevice();
	bool setValue(QString leaf, QString value);

	QStringList objects;
	QStringList leaves;
private:
	QString host;
	QTcpSocket sock;
};

#endif

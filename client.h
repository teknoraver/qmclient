#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>

class Client
{
public:
	Client(QString host);

	QStringList getLines(const QString &cmd);
	void getLeaves(const QString &path);
	void browseDevice();
	bool setValue(const QString &leaf, const QString &value);

	QStringList objects;
	QStringList leaves;
private:
	QString host;
	QTcpSocket sock;
};

#endif

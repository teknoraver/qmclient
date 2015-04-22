#ifndef QSSH_H
#define QSSH_H

#include <libssh2.h>

#include <QTcpSocket>

class QSsh: public QTcpSocket
{
Q_OBJECT

public:
	void connectToHost(const QString &hostName, quint16 port = 22);
	qint64 write(const char *data);
	QByteArray readAll();
	void close();
};

#endif

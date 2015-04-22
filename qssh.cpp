#include <QDebug>

#include "qssh.h"

void QSsh::connectToHost(const QString &hostName, quint16 port)
{
	qDebug() << __func__ << hostName << port;
	QTcpSocket::connectToHost(hostName, port);
}

qint64 QSsh::write(const char *data)
{
	qDebug() << __func__ << data;
	return QTcpSocket::write(data);
}

QByteArray QSsh::readAll()
{
	qDebug() << __func__;
	return QTcpSocket::readAll();
}

void QSsh::close()
{
	QTcpSocket::close();
}

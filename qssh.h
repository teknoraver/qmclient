#ifndef QSSH_H
#define QSSH_H

#include <QTcpSocket>
#include <libssh2.h>

class QSsh: public QTcpSocket
{
Q_OBJECT

public:
	bool connectToHost(const QString &user, const QString &pass, const QString &hostName, quint16 port = 22);
	qint64 write(const char *data) const;
	QByteArray readAll() const;
	void close();

private:
	LIBSSH2_SESSION *session;
	LIBSSH2_CHANNEL *channel;
};

#endif

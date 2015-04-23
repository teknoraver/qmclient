#include <QHostAddress>

#include "qssh.h"

bool QSsh::connectToHost(const QString &user, const QString &pass, const QString &hostName, quint16 port)
{
	QTcpSocket::connectToHost(hostName, port);

	session = NULL;
	channel = NULL;

	session = libssh2_session_init();
	if(!session)
		return false;

	libssh2_session_set_blocking(session, 1);

	if (libssh2_session_startup(session, socketDescriptor()))
		return false;

	if (libssh2_userauth_password(session, user.toUtf8().constData(), pass.toUtf8().constData()))
		return false;

	channel = libssh2_channel_direct_tcpip(session, "127.0.0.1", 9034);

	if(channel == NULL)
		return false;

	return true;
}

qint64 QSsh::write(const char *data)
{
	return libssh2_channel_write(channel, data, qstrlen(data));
}

QByteArray QSsh::readAll()
{
	QByteArray ret;

	while(true) {
		char buffer[16384];
		qint64 readed = libssh2_channel_read(channel, buffer, sizeof(buffer));

		if(readed > 0)
			ret.append(buffer, readed);

		if(readed < (qint64)sizeof(buffer))
			break;
	}

	return ret;
}

void QSsh::close()
{
	if(channel)
		libssh2_channel_free(channel);

	if(session) {
		libssh2_session_disconnect(session, "quit");
		libssh2_session_free(session);
	}
	QTcpSocket::close();
}

#include <QDebug>
#include <QHostAddress>

#include "qssh.h"

void QSsh::connectToHost(const QString &user, const QString &pass, const QString &cmd, const QString &hostName, quint16 port)
{
	qDebug() << __func__ << hostName << port;
	QTcpSocket::connectToHost(hostName, port);

	session = NULL;
	channel = NULL;

	qDebug() << "libssh2_session_init()";
	session = libssh2_session_init();

	qDebug() << "libssh2_session_set_blocking()";
	libssh2_session_set_blocking(session, 1);

	qDebug() << "libssh2_session_startup()";
	if (libssh2_session_startup(session, socketDescriptor())) {
		qDebug() << "Failure establishing SSH session ";
		return;
	}

	if (libssh2_userauth_password(session, user.toUtf8().constData(), pass.toUtf8().constData())) {
		qDebug() << "Authentication by password failed";
		return;
	}

	channel = libssh2_channel_open_session(session);

	if(channel == NULL) {
		qDebug() << "Error libssh2_channel_open_session";
		return;
	}

	if(libssh2_channel_exec(channel, cmd.toUtf8().constData())) {
		qDebug() << "Error libssh2_channel_exec";
		return;
	}
}

qint64 QSsh::write(const char *data)
{
	qDebug() << __func__ << data;
	return libssh2_channel_write(channel, data, qstrlen(data));
}

QByteArray QSsh::readAll()
{
	qDebug() << __func__;
	QByteArray ret;

	while(true) {
		char buffer[16384];
		qint64 readed = libssh2_channel_read(channel, buffer, sizeof(buffer));

		qDebug() << "readed" << readed;
		if(readed > 0)
			ret.append(buffer, readed);

		if(readed < (qint64)sizeof(buffer))
			break;
	}

	qDebug() << "ret" << ret.size();
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

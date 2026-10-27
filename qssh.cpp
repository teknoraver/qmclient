#include <QHostAddress>

#include "qssh.h"

bool QSsh::connectToHost(const QString &user, const QString &pass, const QString &cmd, const QString &hostName, quint16 port)
{
	QTcpSocket::connectToHost(hostName, port);
	waitForConnected(3000);

	session = nullptr;
	channel = nullptr;

	session = libssh2_session_init();
	if(!session)
		goto close_sock;

	libssh2_session_set_blocking(session, 1);
	libssh2_session_set_timeout(session, 5000);

	if (libssh2_session_handshake(session, socketDescriptor()))
		goto free_sess;

	if (libssh2_userauth_password(session, user.toUtf8().constData(), pass.toUtf8().constData()))
		goto disc_sess;

	channel = libssh2_channel_open_session(session);

	if(channel == nullptr)
		goto free_chan;

	if(!libssh2_channel_exec(channel, cmd.toUtf8().constData()))
		return true;

free_chan:
	libssh2_channel_free(channel);
	channel = nullptr;

disc_sess:
	libssh2_session_disconnect(session, "quit");

free_sess:
	libssh2_session_free(session);
	session = nullptr;

close_sock:
	QTcpSocket::close();

	return false;
}

qint64 QSsh::write(QByteArray data) const
{
	return libssh2_channel_write(channel, data.constData(), data.length());
}

QByteArray QSsh::readAll()
{
	QByteArray ret;

	while(true) {
		char buffer[16384];
		qint64 readed = libssh2_channel_read(channel, buffer, sizeof(buffer));

		if(readed < 0) {
			abort();
			break;
		}

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

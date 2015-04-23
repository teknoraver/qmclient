#include <QDebug>
#include <QHostAddress>

#include "qssh.h"

void QSsh::connectToHost(const QString &user, const QString &pass, const QString &hostName, quint16 port)
{
	qDebug() << __func__ << hostName << port;
	QTcpSocket::connectToHost(hostName, port);

	int rc;
	LIBSSH2_SESSION *session;
	LIBSSH2_KNOWNHOSTS *nh;
	size_t len;
	int type;
	const char *fingerprint;

	qDebug() << "libssh2_session_init()";
	session = libssh2_session_init();

	qDebug() << "libssh2_session_set_blocking()";
	libssh2_session_set_blocking(session, 1);

	qDebug() << "libssh2_session_startup()";
	while ((rc = libssh2_session_startup(session, socketDescriptor())) == LIBSSH2_ERROR_EAGAIN);

	if (rc) {
		qDebug() << "Failure establishing SSH session " << rc;
		return;
	}

	qDebug() << "libssh2_knownhost_init()";
	nh = libssh2_knownhost_init(session);

	if(!nh) {
		/* eeek, do cleanup here */
		return;
	}

	/* read all hosts from here */
	qDebug() << "libssh2_knownhost_readfile()";
	libssh2_knownhost_readfile(nh, "known_hosts",
				   LIBSSH2_KNOWNHOST_FILE_OPENSSH);

	/* store all known hosts to here */
	qDebug() << "libssh2_knownhost_writefile()";
	libssh2_knownhost_writefile(nh, "dumpfile", LIBSSH2_KNOWNHOST_FILE_OPENSSH);

	fingerprint = libssh2_session_hostkey(session, &len, &type);

	if(fingerprint) {
		struct libssh2_knownhost *host;
		/* introduced in 1.2.6 */
		int check = libssh2_knownhost_checkp(	nh, peerAddress().toString().toUtf8().constData(), 22,
							fingerprint, len,
							LIBSSH2_KNOWNHOST_TYPE_PLAIN|
							LIBSSH2_KNOWNHOST_KEYENC_RAW,
							&host);
		qDebug("Host check: %d, key: %s\n", check,
			(check <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) ? host->key : "<none>");

	/*****
	 * At this point, we could verify that 'check' tells us the key is
	 * fine or bail out.
	 *****/
	} else {
		/* eeek, do cleanup here */
		qDebug() << "no fingerprint session hostkey";
		return;
	}
	libssh2_knownhost_free(nh);

	/* We could authenticate via password */
	while ((rc = libssh2_userauth_password(session, user.toUtf8().constData(), pass.toUtf8().constData()))
		== LIBSSH2_ERROR_EAGAIN);

	if (rc) {
		qDebug() << "Authentication by password failed";
		return;
	}

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

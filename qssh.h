// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2015 Matteo Croce

#ifndef QSSH_H
#define QSSH_H

#include <QTcpSocket>
#include <libssh2.h>

class QSsh: public QTcpSocket
{
Q_OBJECT

public:
	bool connectToHost(const QString &user, const QString &pass, const QString &cmd, const QString &hostName, quint16 port = 22);
	qint64 write(QByteArray data) const;
	QByteArray readAll();
	void close();

private:
	LIBSSH2_SESSION *session;
	LIBSSH2_CHANNEL *channel;
};

#endif

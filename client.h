// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2015 Matteo Croce

#ifndef CLIENT_H
#define CLIENT_H

#include <QStringList>
#include <QRegularExpression>
#include <QMutex>
#include <QTimer>

#include "qssh.h"

class Client : public QObject
{
Q_OBJECT
public:
	Client(QString host);

	QStringList cmd(const QString &cmd);
	QStringList getLeaves(const QString &path);
	QStringList getMd(const QString &path);
	bool browseDevice();
	bool setValue(const QString &leaf, const QString &value);
	int add(const QString &path);
	bool del(const QString &path);
	void reset();
	void save();
	void reboot();
	void runHandlers(bool run);

	QStringList objects;
private:
	QString host;
	QSsh sock;
	QString e;
	QMutex io;
	QTimer keepalive;

signals:
	void disconnected();

private slots:
	void ping();
};

#endif

/*
 * QmClient - a GUI for Pirelli/ADB CM
 *     Copyright (C) 2015  Matteo Croce
 *
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

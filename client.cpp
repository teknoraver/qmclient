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

#include "client.h"

Client::Client(QString h)
{
	host = h;
	connect(&keepalive, SIGNAL(timeout()), SLOT(ping()));
}

static bool obj_compare(const QString &s1, const QString &s2)
{
	static QRegExp num("\\d+");
	QStringList l1 = s1.split('.');
	QStringList l2 = s2.split('.');

	for(int i = 0; i < qMin(l1.size(), l2.size()); i++)
		if(l1[i] != l2[i]) {
			if(num.exactMatch(l1[i]) && num.exactMatch(l2[i]))
				return l1[i].toInt() < l2[i].toInt();
			else
				return l1[i] < l2[i];
		}

	return l1.size() < l2.size();
}

QStringList Client::cmd(const QString &cmd)
{
	io.lock();
	sock.write((cmd + '\n').toUtf8().constData());
	sock.waitForBytesWritten(-1);

	QByteArray buffer;
	while(true) {
		QByteArray tmp = sock.readAll();
		if(!tmp.size())
			continue;

		buffer += tmp;
		if(!buffer[buffer.size() - 1]) {
			buffer.resize(buffer.size() - 1);
			break;
		}
//		qApp->processEvents();
	}
	io.unlock();
	QStringList ret = QString(buffer).split('\n', QString::SkipEmptyParts);
	return ret;
}

bool Client::browseDevice()
{
	if(sock.state() != QAbstractSocket::UnconnectedState)
		sock.close();

	if(!sock.connectToHost("root", "yaps", host))
		return false;

	sock.setSocketOption(QAbstractSocket::LowDelayOption, 1);
	sock.setSocketOption(QAbstractSocket::KeepAliveOption, true);

	objects = cmd("GETO Device.") + cmd("GETO InternetGatewayDevice.");
	std::sort(objects.begin(), objects.end(), obj_compare);

	keepalive.start(280000);

	return true;
}

QStringList Client::getLeaves(const QString &path)
{
	QStringList ret = cmd("GETD " + path + ". 0");
	ret.sort();
	return ret;
}

QStringList Client::getMd(const QString &path)
{
	return cmd("GETMDP " + path + ". 0");
}

bool Client::setValue(const QString &leaf, const QString &value)
{
	return cmd("SET" + e + ' ' + leaf + ' ' + value).contains("OK");
}

int Client::add(const QString &path)
{
	QStringList ret = cmd("ADD" + e + " " + path);
	if(!ret.contains("ERROR")) {
//		objects << path + '.' + ret[0];
//		objects.sort();
		objects.insert(objects.indexOf(path), path + '.' + ret[0]);
		return ret[0].toInt();
	}
	return -1;
}

bool Client::del(const QString &path)
{
	QStringList ret = cmd("DEL" + e + " " + path);
	if(!ret.contains("ERROR")) {
		foreach(QString s, objects)
			if(s.startsWith(path))
				objects.removeOne(s);
		return true;
	}
	return false;
}
void Client::runHandlers(bool run)
{
	if(run)
		e.clear();
	else
		e = "E";
}

void Client::reset()
{
	cmd("RESET");
	sock.close();
}

void Client::save()
{
	cmd("SAVE");
}

void Client::reboot()
{
	cmd("REBOOT");
}

void Client::ping()
{
	cmd("GETO Device.[]");
}

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

QStringList Client::getLines(const QString &cmd)
{
	sock.write((cmd + '\n').toUtf8().constData());
	sock.waitForBytesWritten(-1);

	QByteArray buffer;
	while(true) {
		sock.waitForReadyRead(1);
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
	QStringList ret = QString(buffer).split('\n', QString::SkipEmptyParts);
	std::sort(ret.begin(), ret.end(), obj_compare);
	return ret;
}

bool Client::browseDevice()
{
	if(sock.state() != QAbstractSocket::UnconnectedState)
		sock.close();

	sock.connectToHost(host, 9034);
	if(!sock.waitForConnected(2000))
		return false;

	sock.setSocketOption(QAbstractSocket::LowDelayOption, 1);
//	sock.write("nc 127.0.0.1 9034");

	objects = getLines("GETO Device.") + getLines("GETO InternetGatewayDevice.");

	return true;
}

void Client::getLeaves(const QString &path)
{
	leaves = getLines("GETD " + path + ". 0");
}

bool Client::setValue(const QString &leaf, const QString &value)
{
	sock.write(("SET" + e + " " + leaf + ' ' + value + '\n').toUtf8().constData());
	sock.waitForBytesWritten(-1);

	QByteArray buffer;
	while(true) {
		sock.waitForReadyRead(1);
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
	if(buffer.startsWith("OK"))
		return true;

	return false;
}

int Client::add(const QString &path)
{
	QStringList ret = getLines("ADD" + e + " " + path);
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
	QStringList ret = getLines("DEL" + e + " " + path);
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
	getLines("RESET");
	sock.close();
}

void Client::save()
{
	getLines("SAVE");
}

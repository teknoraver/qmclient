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
	browseDevice();
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
	ret.sort();
	return ret;
}

void Client::browseDevice()
{
	// if the CM only binds localhost you may want to run this on the device:
	// datapipe 192.168.1.1 9034 127.0.0.1 9034

	if(sock.state() != QAbstractSocket::UnconnectedState)
		sock.close();

	sock.connectToHost(host, 9034);
	if(!sock.waitForConnected(2000)) {
//		QMessageBox::critical(this, "Error", "Can't connect to " + host);
		return;
	}
	sock.setSocketOption(QAbstractSocket::LowDelayOption, 1);

	objects = getLines("GETO Device.");
}

void Client::getLeaves(const QString &path)
{
	leaves = getLines("GETD " + path + ". 0");
}

bool Client::setValue(const QString &leaf, const QString &value)
{

	sock.write((QString("SET ") + leaf + ' ' + value + '\n').toUtf8().constData());
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
	QStringList ret = getLines("ADD " + path);
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
	QStringList ret = getLines("DEL " + path);
	if(!ret.contains("ERROR")) {
		foreach(QString s, objects)
			if(s.startsWith(path))
				objects.removeOne(s);
		return true;
	}
	return false;
}

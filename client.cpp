// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2015 Matteo Croce

#include "client.h"

Client::Client(QString h)
{
	host = h;
	connect(&keepalive, &QTimer::timeout, this, &Client::ping);
}

static bool obj_compare(const QString &s1, const QString &s2)
{
	static const QRegularExpression num(QRegularExpression::anchoredPattern("\\d+"));
	QStringList l1 = s1.split('.');
	QStringList l2 = s2.split('.');

	for(int i = 0; i < qMin(l1.size(), l2.size()); i++)
		if(l1[i] != l2[i]) {
			if(num.match(l1[i]).hasMatch() && num.match(l2[i]).hasMatch())
				return l1[i].toInt() < l2[i].toInt();
			else
				return l1[i] < l2[i];
		}

	return l1.size() < l2.size();
}

QStringList Client::cmd(const QString &cmd)
{
	io.lock();
	sock.write((cmd + '\n' + '\0').toUtf8());
	sock.waitForBytesWritten(-1);

	QByteArray buffer;
	while(true) {
		QByteArray tmp = sock.readAll();

		if(sock.state() == QAbstractSocket::UnconnectedState) {
			emit disconnected();
			break;
		}

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
	QStringList ret = QString(buffer).split('\n', Qt::SkipEmptyParts);
	return ret;
}

bool Client::browseDevice()
{
	if(sock.state() != QAbstractSocket::UnconnectedState)
		sock.close();

	sock.connectToHost("root", "yaps", "exec nc local:/tmp/cmctl", host);
	if(sock.state() != QAbstractSocket::ConnectedState)
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
		objects.removeIf([&path](const QString &s) { return s.startsWith(path); });
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

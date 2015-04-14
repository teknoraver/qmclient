#include "client.h"

Client::Client(QString h)
{
	host = h;
	browseDevice();
}

QStringList Client::getLines(QString cmd)
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

void Client::getLeaves(QString path)
{
	leaves = getLines("GETD " + path + ". 0");
}

bool Client::setValue(QString leaf, QString value)
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

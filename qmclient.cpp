#include <QMessageBox>

#include "qmclient.h"

QmClient::QmClient() : QMainWindow(0)
{
	setupUi(this);
	connect(actionAbout, SIGNAL(triggered()), SLOT(about()));
	connect(actionAbout_Qt, SIGNAL(triggered()), QCoreApplication::instance(), SLOT(aboutQt()));

	sock.connectToHost("127.0.0.1", 9034);
	sock.waitForConnected(-1);
	load("");
	load("WiFi");
}

QmClient::~QmClient()
{
}

void QmClient::load(QString path)
{
	if(!path.isEmpty())
		path += '.';

	qDebug() << "getting" << path;

	sock.write(("GETO Device." + path + "+\n").toUtf8().constData());
	sock.waitForBytesWritten(-1);

	sock.waitForReadyRead(-1);
	QString res = sock.readLine();
	while(!(res = sock.readLine()).isEmpty()) {
		res = res.trimmed();
		if(!res.startsWith("Device." + path))
			continue;
		res = res.remove(QRegularExpression("^Device." + path));
		qDebug() << res;
	}
}

void QmClient::about()
{
        QMessageBox::about(this, "About QmClient", "QmClient - a Qt QmClient application<br>by Matteo Croce <a href=\"http://teknoraver.net/\">http://teknoraver.net/</a>");
}

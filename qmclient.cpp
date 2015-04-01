#include <QMessageBox>

#include "qmclient.h"

QmClient::QmClient() : QMainWindow(0)
{
	setupUi(this);
	connect(actionAbout, SIGNAL(triggered()), SLOT(about()));
	connect(actionAbout_Qt, SIGNAL(triggered()), QCoreApplication::instance(), SLOT(aboutQt()));
	connect(objects, SIGNAL(clicked(const QModelIndex&)), SLOT(selectedObject(const QModelIndex&)));
	connect(browse, SIGNAL(clicked()), SLOT(browseDevice()));

	objects->setModel(&model);
}

QmClient::~QmClient()
{
}

void QmClient::selectedObject(const QModelIndex &index)
{
	QString path = index.data().toString();
	QModelIndex i = index.parent();
	while(i.isValid()) {
		path = i.data().toString() + '.' + path;
		i = i.parent();
	}

//	qDebug() << "load" << path;
	load(path);
}

void QmClient::load(QString path)
{
	objects->setEnabled(false);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	if(!path.isEmpty())
		path += '.';

	sock.write(("GETO " + path + "+\n").toUtf8().constData());
	sock.waitForBytesWritten(-1);

	sock.waitForReadyRead(-1);
	QString res;
	QStringList lines;
	while(!(res = sock.readLine()).isEmpty()) {
		qApp->processEvents();
		res = res.trimmed();
		lines << res;
	}

	// flush pending data, usually zeroes
	if(sock.bytesAvailable())
		sock.readAll();

	if(lines.size() > 1)
		model.setupModelData(lines);

	objects->setExpanded(objects->currentIndex(), true);
	QApplication::setOverrideCursor(Qt::ArrowCursor);
	objects->setEnabled(true);
}

void QmClient::browseDevice()
{
	sock.connectToHost(cpeip->text(), 9034);
	sock.waitForConnected(-1);

	load("Device");
}

void QmClient::about()
{
        QMessageBox::about(this, "About QmClient", "QmClient - a Qt QmClient application<br>by Matteo Croce <a href=\"http://teknoraver.net/\">http://teknoraver.net/</a>");
}

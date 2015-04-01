#include <QMessageBox>

#include "qmclient.h"

QmClient::QmClient() : QMainWindow(0)
{
	setupUi(this);
	connect(actionAbout, SIGNAL(triggered()), SLOT(about()));
	connect(actionAbout_Qt, SIGNAL(triggered()), QCoreApplication::instance(), SLOT(aboutQt()));
	connect(objects, SIGNAL(clicked(const QModelIndex&)), SLOT(selectedObject(const QModelIndex&)));
	connect(browse, SIGNAL(clicked()), SLOT(browseDevice()));

	objects->setModel(&objectsModel);
	leaves->setModel(&leavesModel);
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

	QApplication::setOverrideCursor(Qt::WaitCursor);
	objects->setEnabled(false);
	leaves->setEnabled(false);

	getLeaves(path);

	leaves->setEnabled(true);
	objects->setEnabled(true);
	QApplication::setOverrideCursor(Qt::ArrowCursor);
}

void QmClient::getObjects()
{
	sock.write(QString("GETO Device.\n").toUtf8().constData());
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
		objectsModel.setupModelData(lines);

	objects->setExpanded(objects->currentIndex(), true);
}

void QmClient::getLeaves(QString path)
{
	qDebug() << "getLeaves" << path;
	// flush pending data, usually zeroes
	if(sock.bytesAvailable()) {
		qDebug() << "leftover";
		sock.readAll();
	}
	sock.write(("GETD " + path + ". 0\n").toUtf8().constData());
	sock.waitForBytesWritten(-1);

	sock.waitForReadyRead(-1);
	QString res;
	QStringList lines;
	while(!(res = sock.readLine()).isEmpty()) {
		qApp->processEvents();
		res = res.trimmed();
		lines << res;
		qDebug() << res;
	}
	qDebug() << lines.size() << "leaves";

	leavesModel.fillData(lines);

	// flush pending data, usually zeroes
	if(sock.bytesAvailable()) {
		qDebug() << "leftover";
		sock.readAll();
	}
	qDebug() << "end";
	leaves->resizeColumnsToContents();
}

void QmClient::browseDevice()
{
	sock.connectToHost(cpeip->text(), 9034);
	sock.waitForConnected(-1);

	getObjects();
}

void QmClient::about()
{
        QMessageBox::about(this, "About QmClient", "QmClient - a Qt QmClient application<br>by Matteo Croce <a href=\"http://teknoraver.net/\">http://teknoraver.net/</a>");
}

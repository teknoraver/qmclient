#include <QMessageBox>

#include "qmclient.h"

QmClient::QmClient() : QMainWindow(0)
{
	setupUi(this);
	connect(actionAbout, SIGNAL(triggered()), SLOT(about()));
	connect(actionAbout_Qt, SIGNAL(triggered()), QCoreApplication::instance(), SLOT(aboutQt()));
	connect(objects, SIGNAL(clicked(const QModelIndex&)), SLOT(selectedObject(const QModelIndex&)));
	connect(browse, SIGNAL(clicked()), SLOT(browseDevice()));
	connect(search, SIGNAL(textChanged(const QString&)), SLOT(doSearch(const QString&)));
	connect(leaves, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(doMenu(const QPoint&)));

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
	while(!(res = sock.readLine()).isEmpty()) {
		qApp->processEvents();
		res = res.trimmed();
		objectList << res;
		sock.waitForReadyRead(0);
	}
	objectList.sort();

	// flush pending data, usually zeroes
	if(sock.bytesAvailable())
		sock.readAll();

	objectsModel.setupModelData(objectList);
}

void QmClient::getLeaves(QString path)
{
	//qDebug() << "getLeaves" << path;
	// flush pending data, usually zeroes
	if(sock.bytesAvailable()) {
		//qDebug() << "leftover1";
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
		//qDebug() << res;
		sock.waitForReadyRead(0);
	}
	lines.sort();
	//qDebug() << lines.size() << "leaves";

	leavesModel.fillData(lines);

	// flush pending data, usually zeroes
	if(sock.bytesAvailable()) {
		//qDebug() << "leftover2";
		sock.readAll();
	}
	//qDebug() << "end";
	leaves->resizeColumnsToContents();
}

void QmClient::browseDevice()
{
	sock.connectToHost(cpeip->text(), 9034);
	sock.waitForConnected(-1);

	getObjects();
}

void QmClient::doSearch(const QString &text)
{
	if(text.isEmpty()) {
		objectsModel.setupModelData(objectList);
		objects->collapseAll();
		return;
	}

	objectsModel.setupModelData(objectList.filter(text, Qt::CaseInsensitive));
	objects->expandAll();
}

void QmClient::doMenu(const QPoint &point)
{
	QModelIndex index = leaves->indexAt(point);
	QString val = index.data().toString();

	QMenu menu;
	menu.addAction("Search " + val);
	menu.exec(QCursor::pos());
}

void QmClient::about()
{
        QMessageBox::about(this, "About QmClient", "QmClient - a Qt QmClient application<br>by Matteo Croce <a href=\"http://teknoraver.net/\">http://teknoraver.net/</a>");
}

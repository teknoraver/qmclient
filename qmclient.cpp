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

	leaves->setModel(&leavesModel);
	objectsModel = NULL;
}

QmClient::~QmClient()
{
	if(objectsModel)
		delete objectsModel;
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

QStringList QmClient::getLines(QString cmd)
{
	sock.write(cmd.toUtf8().constData());
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
		qApp->processEvents();
	}
	QStringList ret = QString(buffer).split('\n');
	ret.sort();
	return ret;
}

void QmClient::getObjects()
{
	objectList = getLines("GETO Device.\n");
	objectsModel = new TreeModel(objectList);
	objects->setModel(objectsModel);
}

void QmClient::getLeaves(QString path)
{
	QStringList l = getLines("GETD " + path + ". 0\n");
	leavesModel.fillData(l);
	leaves->resizeColumnsToContents();
}

void QmClient::browseDevice()
{
	sock.connectToHost(cpeip->text(), 9034);
	sock.waitForConnected(-1);
	sock.setSocketOption(QAbstractSocket::LowDelayOption, 1);

	getObjects();
}

void QmClient::doSearch(const QString &text)
{
	if(text.isEmpty()) {
		delete objectsModel;
		objectsModel = new TreeModel(objectList);
		objects->setModel(objectsModel);
		objects->collapseAll();
		return;
	}

	delete objectsModel;
	objectsModel = new TreeModel(objectList.filter(text, Qt::CaseInsensitive));
	objects->setModel(objectsModel);
	objects->expandAll();
}

void QmClient::doMenu(const QPoint &point)
{
	QModelIndex index = leaves->indexAt(point);
	QString val = index.data().toString();

	QMenu menu;

	QAction *searchAct = new QAction("Search " + val, &menu);
	menu.addAction(searchAct);

	QAction *a = menu.exec(QCursor::pos());
	if(a == searchAct)
		search->setText(val);
}

void QmClient::about()
{
        QMessageBox::about(this, "About QmClient", "QmClient - a Qt browser for Pirelli/ADB devices<br>by <a href=\"mailto:matteo@openwrt.org\">Matteo Croce</a>");
}

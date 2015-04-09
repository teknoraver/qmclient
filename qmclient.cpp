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

#include <QMessageBox>

#include "qmclient.h"

QmClient::QmClient()
{
	setupUi(this);
	connect(actionAbout, SIGNAL(triggered()), SLOT(about()));
	connect(actionAbout_Qt, SIGNAL(triggered()), QCoreApplication::instance(), SLOT(aboutQt()));
	connect(objects, SIGNAL(clicked(const QModelIndex&)), SLOT(selectedObject(const QModelIndex&)));
	connect(browse, SIGNAL(clicked()), SLOT(browseDevice()));
	connect(search, SIGNAL(textChanged(const QString&)), SLOT(doSearch(const QString&)));
	connect(objects, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(objectsMenu(const QPoint&)));
	connect(leaves, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(leavesMenu(const QPoint&)));

	leaves->setModel(&leavesModel);
	objectsModel = NULL;

	setClearButton(search);
}

QmClient::~QmClient()
{
	if(objectsModel)
		delete objectsModel;
}

QString QmClient::getFullPath(const QModelIndex &index) const
{
	QString path = index.data().toString();
	QModelIndex i = index.parent();
	while(i.isValid()) {
		path = i.data().toString() + '.' + path;
		i = i.parent();
	}
	return path;
}

void QmClient::selectedObject(const QModelIndex &index)
{
	QString path = getFullPath(index);

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
		qApp->processEvents();
	}
	QStringList ret = QString(buffer).split('\n', QString::SkipEmptyParts);
	ret.sort();
	return ret;
}

void QmClient::getObjects()
{
	objectList = getLines("GETO Device.");
	objectsModel = new TreeModel(objectList);
	objects->setModel(objectsModel);
}

void QmClient::getLeaves(QString path)
{
	QStringList l = getLines("GETD " + path + ". 0");
	leavesModel.fillData(l);
	leaves->resizeColumnsToContents();
}

void QmClient::browseDevice()
{
	// if the CM only binds localhost you may want to run this on the device:
	// datapipe 192.168.1.1 9034 127.0.0.1 9034

	if(sock.state() != QAbstractSocket::UnconnectedState)
		sock.close();

	sock.connectToHost(cpeip->text(), 9034);
	if(!sock.waitForConnected(2000)) {
		QMessageBox::critical(this, "Error", "Can't connect to " + cpeip->text());
		return;
	}
	sock.setSocketOption(QAbstractSocket::LowDelayOption, 1);
	leavesModel.setSocket(&sock);

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

void QmClient::objectsMenu(const QPoint &point)
{
	QModelIndex index = objects->indexAt(point);
	if(!index.isValid())
		return;

	QString val = index.data().toString();

	QMenu menu;

	QAction *addAct = new QAction("Add", &menu);
	menu.addAction(addAct);

	QAction *delAct = new QAction("Delete", &menu);
	menu.addAction(delAct);

	QString path = getFullPath(index);
	QAction *a = menu.exec(QCursor::pos());
	if(a == addAct) {
		QStringList ret = getLines("ADD " + path);
		if(!ret.contains("ERROR")) {
			objectList << path + '.' + ret[0];
			objectList.sort();
			objects->setModel(new TreeModel(objectList));
		}
	} else if(a == delAct) {
		QStringList ret = getLines("DEL " + path);
		if(!ret.contains("ERROR")) {
			QStringList newl;
			foreach(QString s, objectList)
				if(!s.startsWith(path))
					newl << s;
			objectList = newl;
			objects->setModel(new TreeModel(objectList));
		}
	}
}

void QmClient::leavesMenu(const QPoint &point) const
{
	QModelIndex index = leaves->indexAt(point);
	if(!index.isValid())
		return;

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

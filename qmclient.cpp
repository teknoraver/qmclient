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
/*	connect(objects, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(objectsMenu(const QPoint&)));
	connect(leaves, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(leavesMenu(const QPoint&)));*/

	setClearButton(search);
}

QmClient::~QmClient()
{
}

void QmClient::browseDevice()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	client = new Client(cpeip->text());
	leavesModel = new Leaves(client);
	treeModel = new TreeModel(client, this);

	leaves->setModel(leavesModel);
	objects->setModel(treeModel);

	QApplication::setOverrideCursor(Qt::ArrowCursor);
}

void QmClient::selectedObject(const QModelIndex &index)
{
	QString path = getFullPath(index);

	qDebug() << "sel" << path;

	QApplication::setOverrideCursor(Qt::WaitCursor);
	objects->setEnabled(false);
	leaves->setEnabled(false);

	leavesModel->loadPath(path);

	leaves->setEnabled(true);
	objects->setEnabled(true);
	QApplication::setOverrideCursor(Qt::ArrowCursor);
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

void QmClient::doSearch(const QString &text)
{
	if(text.isEmpty()) {
		treeModel->filter(QString());
		objects->collapseAll();
		return;
	}

	treeModel->filter(text);
	objects->expandAll();
}

void QmClient::about()
{
	QMessageBox::about(this, "About QmClient", "QmClient - a Qt browser for Pirelli/ADB devices<br>by <a href=\"mailto:matteo@openwrt.org\">Matteo Croce</a>");
}

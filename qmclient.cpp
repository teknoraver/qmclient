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
#include <QClipboard>
#include <QCompleter>
#include <QSettings>

#include "qmclient.h"

QmClient::QmClient() : client(0), leavesModel(0), treeModel(0)
{
	setupUi(this);
	connect(actionAbout, SIGNAL(triggered()), SLOT(about()));
	connect(actionAbout_Qt, SIGNAL(triggered()), QCoreApplication::instance(), SLOT(aboutQt()));
	connect(objects, SIGNAL(clicked(const QModelIndex&)), SLOT(selectedObject(const QModelIndex&)));
	connect(browse, SIGNAL(clicked()), SLOT(browseDevice()));
	connect(search, SIGNAL(textChanged(const QString&)), SLOT(doSearch(const QString&)));
	connect(searchType, SIGNAL(currentIndexChanged(int)), SLOT(changeSearch()));
	connect(leaves, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(leavesMenu(const QPoint&)));
	connect(objects, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(objectsMenu(const QPoint&)));
	connect(actionHandlers, SIGNAL(toggled(bool)), SLOT(runHandlers(bool)));
	connect(actionCmReset, SIGNAL(triggered()), SLOT(cmReset()));
	connect(actionCmSave, SIGNAL(triggered()), SLOT(cmSave()));
	connect(actionCmReboot, SIGNAL(triggered()), SLOT(cmReboot()));

	setClearButton(search);

	QSettings settings;
	int size = settings.beginReadArray("recents");
	for (int i = 0; i < size; i++) {
		settings.setArrayIndex(i);
		recents << settings.value("cpeip").toString();
	}
	settings.endArray();
	cpeip->setCompleter(new QCompleter(recents, cpeip));
}

QmClient::~QmClient()
{
	resetPointers();
}

void QmClient::browseDevice()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	resetPointers();
	disableAll();

	client = new Client(cpeip->text());
	if(client->browseDevice()) {
		leavesModel = new Leaves(client);
		connect(leavesModel, SIGNAL(dataAboutToBeChanged()), SLOT(disableAll()));
		connect(leavesModel, dataChangedQtx(), SLOT(enableAll()));
		connect(client, SIGNAL(disconnected()), SLOT(disconnected()));
		treeModel = new TreeModel(client, this);

		client->runHandlers(actionHandlers->isChecked());

		leaves->setModel(leavesModel);
		leaves->setItemDelegate(&delegate);
		objects->setModel(treeModel);
		if(!recents.contains(cpeip->text(), Qt::CaseInsensitive)) {
			recents << cpeip->text();
			if(recents.size() > 4)
				recents.removeFirst();

			QSettings settings;
			settings.beginWriteArray("recents");
			for(int i = 0; i < recents.size(); i++) {
				settings.setArrayIndex(i);
				settings.setValue("cpeip", recents[i]);
			}
			settings.endArray();
			cpeip->setCompleter(new QCompleter(recents, cpeip));
		}
	} else {
		QMessageBox::critical(this, "Error", "Can't connect to " + cpeip->text());
		delete client;
		client = NULL;
	}

	enableAll();
}

void QmClient::selectedObject(const QModelIndex &index)
{
	QString path = getFullPath(index);

	disableAll();

	leavesModel->loadPath(path);
	if(leavesModel) {
		leaves->verticalHeader()->setVisible(leavesModel->enumeration);
		leaves->resizeColumnsToContents();
	}

	enableAll();
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
	if(!client)
		return;

	if(text.isEmpty()) {
		treeModel->filter(QRegExp());
		objects->collapseAll();
		return;
	}

	QRegExp::PatternSyntax ps;
	switch(searchType->currentIndex()) {
	default:
	case 0:	ps = QRegExp::FixedString;
		break;
	case 1:	ps = QRegExp::WildcardUnix;
		break;
	case 2:	ps = QRegExp::RegExp;
		break;
	}
	treeModel->filter(QRegExp(text, Qt::CaseInsensitive, ps));
	objects->expandAll();
}

void QmClient::changeSearch()
{
	doSearch(search->text());
}

void QmClient::leavesMenu(const QPoint &point) const
{
	QModelIndex index = leaves->indexAt(point);
	if(!index.isValid())
		return;

	QString val = index.data().toString();

	QMenu menu;

	QAction *copyAct = new QAction(QIcon(":icons/copy.png"), "&Copy path", &menu);
	menu.addAction(copyAct);

	QAction *searchAct = NULL;
	if(val.startsWith("Device.") || val.startsWith("InternetGatewayDevice.")) {
		searchAct = new QAction(QIcon(":icons/search.png"), "&Search " + val, &menu);
		menu.addAction(searchAct);
	}

	QAction *a = menu.exec(QCursor::pos());
	if(!a)
		return;
	else if(a == copyAct)
		QApplication::clipboard()->setText(index.data(Qt::UserRole).toString());
	else if(a == searchAct) {
		searchType->setCurrentIndex(0);
		search->setText(val);
	}
}

void QmClient::objectsMenu(const QPoint &point)
{
	QModelIndex index = objects->indexAt(point);
	if(!index.isValid())
		return;

	QMenu menu;

	QAction *copyAct = new QAction(QIcon(":icons/copy.png"), "&Copy path", &menu);
	menu.addAction(copyAct);

	QAction *addAct = new QAction(QIcon(":icons/add.png"), "&Add", &menu);
	menu.addAction(addAct);

	QAction *delAct = new QAction(QIcon(":icons/del.png"), "&Delete", &menu);
	menu.addAction(delAct);

	QAction *a = menu.exec(QCursor::pos());

	if(!a)
		return;
	else if(a == copyAct)
		QApplication::clipboard()->setText(getFullPath(index));
	else {
		disableAll();
		if(a == addAct)
			treeModel->insertRow(0, index);
		else if(a == delAct)
			treeModel->removeRow(index.row(), index);
		enableAll();
	}
}

void QmClient::runHandlers(bool run) const
{
	if(client)
		client->runHandlers(run);
}

void QmClient::cmReset()
{
	if(client &&
		QMessageBox::question(this, "Reset", "Do you really want to reset the CPE to default settings?",
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::No) == QMessageBox::Yes) {
		client->reset();
		resetPointers();
	}
}

void QmClient::cmSave() const
{
	if(client) {
		disableAll();
		client->save();
		enableAll();
	}
}

void QmClient::cmReboot()
{
	if(client) {
		client->reset();
		resetPointers();
	}
}

void QmClient::resetPointers()
{
	if(client) {
		delete client;
		client = NULL;
	}
	if(leavesModel) {
		delete leavesModel;
		leavesModel = NULL;
	}
	if(treeModel) {
		delete treeModel;
		treeModel = NULL;
	}
}

void QmClient::disconnected()
{
	QMessageBox::critical(this, "Error", "Network error");
	enableAll();
}

void QmClient::disableAll() const
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
	objects->setEnabled(false);
	leaves->setEnabled(false);
	qApp->processEvents();
}

void QmClient::enableAll() const
{
	leaves->setEnabled(true);
	objects->setEnabled(true);
	QApplication::restoreOverrideCursor();
	QApplication::restoreOverrideCursor();
}

void QmClient::about()
{
	QMessageBox::about(this, "About QmClient", "QmClient " VERSION " - a Qt browser for Pirelli/ADB devices<br>by <a href=\"mailto:matteo@openwrt.org\">Matteo Croce</a>");
}

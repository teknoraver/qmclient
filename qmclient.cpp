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

QmClient::QmClient() : client(nullptr), leavesModel(nullptr), treeModel(nullptr)
{
	setupUi(this);
	connect(actionAbout, &QAction::triggered, this, &QmClient::about);
	connect(actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);
	connect(objects, &QAbstractItemView::clicked, this, &QmClient::selectedObject);
	connect(browse, &QAbstractButton::clicked, this, &QmClient::browseDevice);
	connect(search, &QLineEdit::textChanged, this, &QmClient::doSearch);
	connect(searchType, &QComboBox::currentIndexChanged, this, &QmClient::changeSearch);
	connect(leaves, &QWidget::customContextMenuRequested, this, &QmClient::leavesMenu);
	connect(objects, &QWidget::customContextMenuRequested, this, &QmClient::objectsMenu);
	connect(actionHandlers, &QAction::toggled, this, &QmClient::runHandlers);
	connect(actionCmReset, &QAction::triggered, this, &QmClient::cmReset);
	connect(actionCmSave, &QAction::triggered, this, &QmClient::cmSave);
	connect(actionCmReboot, &QAction::triggered, this, &QmClient::cmReboot);

	search->setClearButtonEnabled(true);

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
		connect(leavesModel, &Leaves::dataAboutToBeChanged, this, &QmClient::disableAll);
		connect(leavesModel, &QAbstractItemModel::dataChanged, this, &QmClient::enableAll);
		connect(client, &Client::disconnected, this, &QmClient::disconnected);
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
		client = nullptr;
	}

	enableAll();
}

void QmClient::selectedObject(const QModelIndex &index)
{
	QString path = getFullPath(index);
	statusbar->showMessage(path);

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
		treeModel->filter(QRegularExpression());
		objects->collapseAll();
		return;
	}

	QString pattern;
	switch(searchType->currentIndex()) {
	default:
	case 0:	pattern = QRegularExpression::escape(text);
		break;
	case 1:	pattern = QRegularExpression::wildcardToRegularExpression(text,
			QRegularExpression::UnanchoredWildcardConversion | QRegularExpression::NonPathWildcardConversion);
		break;
	case 2:	pattern = text;
		break;
	}
	treeModel->filter(QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption));
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

	QAction *searchAct = nullptr;
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
		client->reboot();
		resetPointers();
	}
}

void QmClient::resetPointers()
{
	if(client) {
		delete client;
		client = nullptr;
	}
	if(leavesModel) {
		delete leavesModel;
		leavesModel = nullptr;
	}
	if(treeModel) {
		delete treeModel;
		treeModel = nullptr;
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

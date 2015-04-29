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

#ifndef QMCLIENT_H
#define QMCLIENT_H

#include <QMainWindow>

#include "ui_qmclientwidget.h"

#include "compat.h"
#include "treemodel.h"
#include "leaves.h"
#include "client.h"
#include "mddelegate.h"

class QmClient: public QMainWindow, private Ui::MainWindow
{
Q_OBJECT
public:
	QmClient();
	~QmClient();

private:
	Client *client;
	Leaves *leavesModel;
	TreeModel *treeModel;
	MetaDataDelegate delegate;
	QStringList recents;

	QString getFullPath(const QModelIndex &index) const;
	void resetPointers();

private slots:
	void browseDevice();
	void selectedObject(const QModelIndex&);
	void doSearch(const QString&);
	void changeSearch();
	void leavesMenu(const QPoint&) const;
	void objectsMenu(const QPoint&);
	void runHandlers(bool) const;
	void disableAll() const;
	void enableAll() const;
	void cmReset();
	void cmSave() const;
	void about();
};

#endif

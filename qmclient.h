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
#include <QTcpSocket>

#include "ui_qmclientwidget.h"

#include "compat.h"
#include "treemodel.h"
#include "leaves.h"

class QmClient: public QMainWindow, private Ui::MainWindow
{
Q_OBJECT
public:
	QmClient();
	~QmClient();

private:
	QTcpSocket sock;
	void getObjects();
	void getLeaves(QString);
	QStringList getLines(QString);
	TreeModel *objectsModel;
	Leaves leavesModel;
	QStringList objectList;

private slots:
	QString getFullPath(const QModelIndex&) const;
	void selectedObject(const QModelIndex&);
	void browseDevice();
	void doSearch(const QString&);
	void objectsMenu(const QPoint&);
	void leavesMenu(const QPoint&) const;
	void about();
};

#endif

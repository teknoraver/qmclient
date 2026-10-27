// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2015 Matteo Croce

#ifndef QMCLIENT_H
#define QMCLIENT_H

#include <QMainWindow>

#include "ui_qmclientwidget.h"

#include "treemodel.h"
#include "leaves.h"
#include "client.h"
#include "mddelegate.h"

class QmClient: public QMainWindow, private Ui::MainWindow
{
Q_OBJECT
public:
	QmClient();
	~QmClient() override;

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
	void cmReboot();
	void disconnected();
	void about();
};

#endif

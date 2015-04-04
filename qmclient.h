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

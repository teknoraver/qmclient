#ifndef QMCLIENT_H
#define QMCLIENT_H

#include <QMainWindow>
#include <QTcpSocket>

#include "ui_qmclientwidget.h"

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
	TreeModel objectsModel;
	Leaves leavesModel;
	QStringList objectList;

private slots:
	void selectedObject(const QModelIndex&);
	void browseDevice();
	void doSearch(const QString&);
	void doMenu(const QPoint&);
	void about();
};

#endif

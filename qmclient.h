#ifndef QMCLIENT_H
#define QMCLIENT_H

#include <QMainWindow>
#include <QTcpSocket>

#include "ui_qmclientwidget.h"

#include "treemodel.h"

class QmClient: public QMainWindow, private Ui::MainWindow
{
Q_OBJECT
public:
	QmClient();
	~QmClient();

private:
	QTcpSocket sock;
	void getObjects(QString);
	void getLeaves(QString);
	TreeModel model;

private slots:
	void selectedObject(const QModelIndex&);
	void browseDevice();
	void about();
};

#endif

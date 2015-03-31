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
	void load(QString);
	TreeModel model;

private slots:
	void about();
	void selectedObject(const QModelIndex&);
};

#endif

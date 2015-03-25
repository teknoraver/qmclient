#ifndef QMCLIENT_H
#define QMCLIENT_H

#include <QMainWindow>

#include "ui_qmclientwidget.h"

class QmClient: public QMainWindow, private Ui::MainWindow
{
	Q_OBJECT
public:
	QmClient();
	~QmClient();
private slots:
	void about();
};

#endif

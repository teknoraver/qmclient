#include <QMessageBox>

#include "qmclient.h"

QmClient::QmClient() : QMainWindow(0)
{
	setupUi(this);
	connect(actionAbout, SIGNAL(triggered()), SLOT(about()));
	connect(actionAbout_Qt, SIGNAL(triggered()), QCoreApplication::instance(), SLOT(aboutQt()));
}

QmClient::~QmClient()
{
}

void QmClient::about()
{
        QMessageBox::about(this, "About QmClient", "QmClient - a Qt QmClient application<br>by Matteo Croce <a href=\"http://teknoraver.net/\">http://teknoraver.net/</a>");
}

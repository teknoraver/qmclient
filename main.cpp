// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2015 Matteo Croce

#include <QApplication>
#include "qmclient.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setOrganizationName("teknoraver");
	a.setApplicationName("QmClient");
	QmClient app;
	app.show();
	return a.exec();
}

FORMS += qmclientwidget.ui
HEADERS += compat.h qmclient.h treeitem.h treemodel.h leaves.h client.h qssh.h mddelegate.h leaf.h
SOURCES += main.cpp qmclient.cpp treeitem.cpp treemodel.cpp leaves.cpp client.cpp qssh.cpp mddelegate.cpp
RESOURCES += qmclient.qrc
QMCLIENT = app
CONFIG += release warn_on thread qt
TARGET = qmclient
QT += widgets network
RC_FILE = qmclient.rc
LIBS += -lssh2

VER = $$system(grep -m1 -o '[0-9.]*' debian/changelog)
DEFINES += VERSION=\"\\\"$${VER}\\\"\"

win32 {
	LIBS += -lcrypto -static
}

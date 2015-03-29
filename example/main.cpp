#include <QApplication>
#include <QFile>
#include <QTreeView>
#include <QDesktopWidget>

#include "treemodel.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QFile file("default.txt");
	file.open(QIODevice::ReadOnly);
	TreeModel model(file.readAll());
	file.close();

	QTreeView view;
	view.setModel(&model);
	view.setWindowTitle(QObject::tr("Simple Tree Model"));
	view.resize(640, 480);
	view.show();
	view.move((QApplication::desktop()->screenGeometry(&view).width() - view.width()) / 2,
		  (QApplication::desktop()->screenGeometry(&view).height() - view.height()) / 2);
	return app.exec();
}

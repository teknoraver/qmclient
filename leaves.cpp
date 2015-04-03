#include <QModelIndex>
//#include <QDebug>

#include "leaves.h"

Leaves::Leaves()
{
	sock = NULL;
}

int Leaves::rowCount(const QModelIndex&) const
{
	return leaves.size();
}

int Leaves::columnCount(const QModelIndex&) const
{
	return 2;
}

QVariant Leaves::data(const QModelIndex &index, int role) const
{
	if(role == Qt::DisplayRole) {
		switch(index.column()) {
		case 0: return leaves[index.row()].mid(leaves[index.row()].lastIndexOf('.') + 1);
		case 1: return values[index.row()];
		}
	}

	return QVariant::Invalid;
}

void Leaves::fillData(QStringList &data)
{
	leaves.clear();
	values.clear();

	foreach(QString leaf, data) {
		QStringList tokens = leaf.split(';');
		if(tokens.size() >= 2) {
			leaves << tokens[0];
			values << tokens[1];
		}
	}
	emit layoutChanged();
}

Qt::ItemFlags Leaves::flags(const QModelIndex &index) const
{
	Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;

	if(index.column() == 1)
		flags |= Qt::ItemIsEditable;

	return flags;
}

bool Leaves::setData(const QModelIndex &index, const QVariant &value, int)
{
	if(sock) {
		sock->write((QString("SET ") + leaves[index.row()] + ' ' + value.toString() + '\n').toUtf8().constData());
		sock->waitForBytesWritten(-1);

		QByteArray buffer;
		while(true) {
			sock->waitForReadyRead(1);
			QByteArray tmp = sock->readAll();
			if(!tmp.size())
				continue;

			buffer += tmp;
			if(!buffer[buffer.size() - 1]) {
				buffer.resize(buffer.size() - 1);
				break;
			}
//			qApp->processEvents();
		}
		if(buffer.startsWith("OK")) {
			values[index.row()] = value.toString();
			emit dataChanged(index, index);
			return true;
		}
	}
	return false;
}

void Leaves::setSocket(QTcpSocket *s)
{
	sock = s;
}

#include <QModelIndex>
//#include <QDebug>

#include "leaves.h"

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
		case 0: return leaves[index.row()];
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
		leaves << tokens[0];
		values << tokens[1];
	}
	emit layoutChanged();
}

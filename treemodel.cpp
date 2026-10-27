// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2015 Matteo Croce

#include <QStringList>

#include "treeitem.h"
#include "treemodel.h"

TreeModel::TreeModel(Client *c, QObject *parent) : QAbstractItemModel(parent), rootItem(nullptr)
{
	client = c;
	setupModelData(client->objects);
}

TreeModel::~TreeModel()
{
	delete rootItem;
}

int TreeModel::columnCount(const QModelIndex &) const
{
	return 1;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole)
		return QVariant();

	TreeItem *item = static_cast<TreeItem *>(index.internalPointer());

	return item->data();
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return rootItem->data();

	return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	TreeItem *parentItem;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<TreeItem *>(parent.internalPointer());

	TreeItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	TreeItem *childItem = static_cast<TreeItem *>(index.internalPointer());
	TreeItem *parentItem = childItem->parentItem();

	if (parentItem == rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
	TreeItem *parentItem;
	if (!rootItem || parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<TreeItem *>(parent.internalPointer());

	return parentItem->childCount();
}

void TreeModel::add(const QString &lineData, int position, QList<TreeItem *> &parents, QList<int>&indentations) const
{
	if (position > indentations.last()) {
		// The last child of the current parent is now the new parent
		// unless the current parent has no children.

		if (parents.last()->childCount() > 0) {
			parents << parents.last()->child(parents.last()->childCount() - 1);
			indentations << position;
		}
	} else {
		while (position < indentations.last() && parents.count() > 0) {
			parents.pop_back();
			indentations.pop_back();
		}
	}

	// Append a new item to the current parent's list of children.
	parents.last()->appendChild(new TreeItem(lineData));
}

void TreeModel::filter(const QRegularExpression &filter)
{
	if(filter.pattern().isEmpty())
		setupModelData(client->objects);
	else if(!filter.isValid())
		setupModelData(QStringList());
	else
		setupModelData(client->objects.filter(filter));
}

bool TreeModel::insertRow(int, const QModelIndex &parent)
{
	TreeItem *item = getItem(parent);
	int i = client->add(item->path());
	if(i > 0) {
		beginInsertRows(parent, 0, 0);
		item->appendChild(new TreeItem(QString::number(i)));
		endInsertRows();
		return true;
	}
	return false;
}

bool TreeModel::removeRow(int row, const QModelIndex &parent)
{
	TreeItem *item = getItem(parent);
	if(client->del(item->path())) {
		beginRemoveRows(parent.parent(), row, row);
		item->parentItem()->delChild(row);
		endRemoveRows();
		return true;
	}
	return false;
}

TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
	if (index.isValid()) {
		TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
		if (item)
		return item;
	}
	return rootItem;
}

void TreeModel::setupModelData(const QStringList &lines)
{
	beginResetModel();
	if(rootItem)
		delete rootItem;

	rootItem = new TreeItem(QString());
	QList<TreeItem *>parents;
	QList<int>indentations;
	parents << rootItem;
	indentations << 0;

	for (int number = 0; number < lines.count(); number++) {
		if (lines[number].isEmpty() || lines[number][0] == '#')
			continue;

		QStringList tokens = lines[number].split('.', Qt::SkipEmptyParts);
		for(int i = 0; i < tokens.size(); i++)
			if(!rootItem->find(tokens.mid(0, i + 1)))
				add(tokens[i], i, parents, indentations);
	}
	endResetModel();
	emit layoutChanged();
}

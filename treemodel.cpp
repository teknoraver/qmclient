/*
 * QmClient - a GUI for Pirelli/ADB CM
 *     Copyright (C) 2015  Matteo Croce
 *
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QStringList>

#include "treeitem.h"
#include "treemodel.h"

TreeModel::TreeModel(Client *c, QObject *parent) : QAbstractItemModel(parent)
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
		return 0;

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

void TreeModel::filter(const QString &filter)
{
	if(filter.isEmpty())
		setupModelData(client->objects);
	else
		setupModelData(client->objects.filter(filter, Qt::CaseInsensitive));
}

void TreeModel::addObject(const QString &path)
{
	if(client->add(path) > 0)
		setupModelData(client->objects);
}

void TreeModel::delObject(const QString &path)
{
	if(client->del(path))
		setupModelData(client->objects);
}

void TreeModel::setupModelData(const QStringList &lines)
{
	rootItem = new TreeItem("Device");
	QList<TreeItem *>parents;
	QList<int>indentations;
	parents << rootItem;
	indentations << 0;

	for (int number = 0; number < lines.count(); number++) {
		if (lines[number].isEmpty() || lines[number][0] == '#')
			continue;

		QStringList tokens = lines[number].split('.', QString::SkipEmptyParts);

		for(int i = 0; i < tokens.size(); i++)
			if(!rootItem->find(tokens.mid(0, i + 1)))
				add(tokens[i], i, parents, indentations);
	}

	emit layoutChanged();
}

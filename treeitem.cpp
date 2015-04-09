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

TreeItem::TreeItem(const QString &data, TreeItem *parent)
{
	m_parentItem = parent;
	m_itemData = data;
}

TreeItem::~TreeItem()
{
	qDeleteAll(m_childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
	item->m_parentItem = this;
	m_childItems.append(item);
}

TreeItem *TreeItem::child(int row) const
{
	return m_childItems.value(row);
}

int TreeItem::childCount() const
{
	return m_childItems.count();
}

int TreeItem::columnCount() const
{
	return 1;
}

QVariant TreeItem::data() const
{
	return m_itemData;
}

TreeItem *TreeItem::parentItem() const
{
	return m_parentItem;
}

int TreeItem::row() const
{
	if (m_parentItem)
		return m_parentItem->m_childItems.indexOf(const_cast<TreeItem *>(this));

	return 0;
}

TreeItem* TreeItem::find(QStringList path)
{
	if(path.isEmpty())
		return this;

	foreach(TreeItem *child, m_childItems)
		if(child->m_itemData == path[0]) {
			path.removeFirst();
			return child->find(path);
		}

	return NULL;
}

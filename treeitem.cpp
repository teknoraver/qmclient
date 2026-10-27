// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2015 Matteo Croce

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

void TreeItem::delChild(int row)
{
	delete m_childItems.takeAt(row);
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

	for(TreeItem *child : std::as_const(m_childItems))
		if(child->m_itemData == path[0]) {
			path.removeFirst();
			return child->find(path);
		}

	return nullptr;
}

QString TreeItem::path() const
{
	if(m_parentItem && !m_parentItem->m_itemData.isEmpty())
		return m_parentItem->path() + '.' + m_itemData;
	else
		return m_itemData;
}

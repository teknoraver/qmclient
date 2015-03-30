#include <QStringList>
#include <QDebug>

#include "treeitem.h"

TreeItem::TreeItem(const QString & data, TreeItem * parent)
{
	m_parentItem = parent;
	m_itemData = data;
}

TreeItem::~TreeItem()
{
	qDeleteAll(m_childItems);
}

void TreeItem::appendChild(TreeItem * item)
{
	m_childItems.append(item);
}

TreeItem *TreeItem::child(int row)
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

TreeItem *TreeItem::parentItem()
{
	return m_parentItem;
}

int TreeItem::row() const
{
	if (m_parentItem)
		return m_parentItem->m_childItems.indexOf(const_cast < TreeItem * >(this));

	return 0;
}

TreeItem* TreeItem::find(QStringList path)
{
	if(path.isEmpty())
		return this;

	foreach(TreeItem *child, m_childItems)
		if(child->data() == path[0]) {
			path.removeFirst();
			return child->find(path);
		}

	return NULL;
}

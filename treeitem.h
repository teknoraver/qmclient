// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2015 Matteo Croce

#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QStringList>
#include <QVariant>

class TreeItem
{
public:
	TreeItem(const QString &data, TreeItem *parentItem = nullptr);
	~TreeItem();

	void appendChild(TreeItem *child);
	void delChild(int row);

	TreeItem *child(int row) const;
	int childCount() const;
	int columnCount() const;
	QVariant data() const;
	int row() const;
	TreeItem* parentItem() const;
	TreeItem* find(QStringList path);
	QString path() const;

private:
	QList<TreeItem *>m_childItems;
	QString m_itemData;
	TreeItem *m_parentItem;
};

#endif

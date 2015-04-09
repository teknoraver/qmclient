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

#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QStringList>
#include <QVariant>

class TreeItem {
public:
	TreeItem(const QString &data, TreeItem *parentItem = 0);
	~TreeItem();

	void appendChild(TreeItem *child);

	TreeItem *child(int row) const;
	int childCount() const;
	int columnCount() const;
	QVariant data() const;
	int row() const;
	TreeItem* parentItem() const;
	TreeItem* find(QStringList path);

private:
	QList<TreeItem *>m_childItems;
	QString m_itemData;
	TreeItem *m_parentItem;
};

#endif

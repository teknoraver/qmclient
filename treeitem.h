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

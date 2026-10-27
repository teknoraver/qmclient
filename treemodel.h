// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2015 Matteo Croce

#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QRegularExpression>

#include "treeitem.h"
#include "client.h"

class TreeModel:public QAbstractItemModel
{
Q_OBJECT
public:
	TreeModel(Client *c, QObject *parent = nullptr);
	~TreeModel() override;

	void filter(const QRegularExpression &filter);
	bool insertRow(int row, const QModelIndex &parent = QModelIndex());
	bool removeRow(int row, const QModelIndex &parent = QModelIndex());

private:
	QVariant data(const QModelIndex &index, int role) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	TreeItem* getItem(const QModelIndex &index) const;
	void add(const QString &obj, int position, QList<TreeItem *> &parents, QList<int>&indentations) const;
	void setupModelData(const QStringList &lines);

	Client *client;
	TreeItem *rootItem;
};

#endif

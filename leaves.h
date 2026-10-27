// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2015 Matteo Croce

#ifndef LEAVES_H
#define LEAVES_H

#include <QTcpSocket>
#include <QStringList>
#include <QAbstractTableModel>

#include "client.h"
#include "leaf.h"

class Leaves : public QAbstractTableModel
{
Q_OBJECT
public:
	Leaves(Client *c);
	void loadPath(QString path);
	bool enumeration = false;

signals:
	void dataAboutToBeChanged();

private:
	int rowCount(const QModelIndex&) const override;
	int columnCount(const QModelIndex&) const override;
	QVariant data(const QModelIndex&, int) const override;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QList<Leaf> leaves;
	Client *client;
};

#endif

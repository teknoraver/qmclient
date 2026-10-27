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
	bool enumeration;

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

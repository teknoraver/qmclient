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

#include <QModelIndex>

#include "leaves.h"

Leaves::Leaves(Client *c)
{
	client = c;
}

int Leaves::rowCount(const QModelIndex&) const
{
	return leaves.size();
}

int Leaves::columnCount(const QModelIndex&) const
{
	return 2;
}

QVariant Leaves::data(const QModelIndex &index, int role) const
{
	if(role == Qt::DisplayRole) {
		switch(index.column()) {
		case 0: return leaves[index.row()].mid(leaves[index.row()].lastIndexOf('.') + 1);
		case 1: return values[index.row()];
		}
	}

	return QVariant::Invalid;
}

void Leaves::loadPath(QString path)
{
	leaves.clear();
	values.clear();

	client->getLeaves(path);
	foreach(QString leaf, client->leaves) {
		QStringList tokens = leaf.split(';');
		if(tokens.size() >= 2) {
			leaves << tokens[0];
			values << tokens[1];
		}
	}
	emit layoutChanged();
}

Qt::ItemFlags Leaves::flags(const QModelIndex &index) const
{
	Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	if(index.column() == 1)
		flags |= Qt::ItemIsEditable;

	return flags;
}

bool Leaves::setData(const QModelIndex &index, const QVariant &value, int)
{
	bool updated = client->setValue(leaves[index.row()], value.toString());

	if(updated) {
		values[index.row()] = value.toString();
		emit dataChanged(index, index);
	}

	return updated;
}

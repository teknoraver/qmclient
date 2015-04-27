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
	Leave l = leaves[index.row()];
	switch(role) {
	case Qt::DisplayRole:
	case Qt::EditRole:
		switch(index.column()) {
		case 0: return l.name.mid(l.name.lastIndexOf('.') + 1);
		case 1: return l.value;
		}
	case Qt::UserRole:
		return l.name;
	}

	return QVariant::Invalid;
}

void Leaves::loadPath(QString path)
{
	leaves.clear();

	QHash<QString, QStringList> mdata;
	QStringList md = client->getMd(path);
	QString last;
	foreach(QString line, md) {
		if(line.startsWith('	'))
			mdata[last] << line.trimmed();
		else {
			last = line;
			mdata[last] = QStringList();
		}
	}

	foreach(QString leaf, client->getLeaves(path)) {
		QStringList tokens = leaf.split(';');
		if(tokens.size() >= 2) {
			Leave::Type type = Leave::String;
			QStringList values;
			foreach(QString s, mdata[tokens[0]]) {
				if(s == "type:boolean") {
					type = Leave::Enum;
					values << "true" << "false";
					break;
				} else if(s == "type:unsignedInt") {
					type = Leave::Number;
					break;
				} else if(s == "type:dateTime") {
					type = Leave::Date;
					break;
				} else if(s.startsWith("type:string")) {
					type = Leave::String;
				} else if(s.startsWith("enum:")) {
					type = Leave::Enum;
					values << s.mid(s.indexOf(':') + 1);
				} else if(s.startsWith("pathref:targetParent:")) {
					type = Leave::Enum;

					QString ref = s.mid(s.lastIndexOf(':') + 1);
					if(ref.endsWith('.'))
						ref = ref.left(ref.size() - 1);

					if(ref.startsWith('.'))
						ref = tokens[0].mid(0, tokens[0].indexOf('.')) + ref;
					else if(ref.startsWith('#')) {
						QString unenum = path.remove(QRegExp("\\.\\d+"));
						QString basename = ref.mid(ref.lastIndexOf('#') + 1);
						QStringList pathlist = unenum.split('.');
						int hashes = qMin(pathlist.size(), ref.count('#'));
						ref = QStringList(pathlist.mid(0, pathlist.size() - hashes)).join(".") + basename;
					}

					ref = ref.remove(".{i}");
					ref = ref.replace('.', "\\.(\\d+\\.|)");
					ref = '^' + ref + "\\.\\d+$";
					values << client->objects.filter(QRegExp(ref));
				}
			}
			leaves << Leave(tokens[0], tokens[1], type, values);
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

QModelIndex Leaves::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	return createIndex(row, column, (void *)&leaves[row]);
}

bool Leaves::setData(const QModelIndex &index, const QVariant &value, int)
{
	emit dataAboutToBeChanged();
	bool updated = client->setValue(leaves[index.row()].name, value.toString());

	if(updated)
		leaves[index.row()].value = value.toString();

	emit dataChanged(index, index);
	return updated;
}

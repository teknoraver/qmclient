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
	Leaf l = leaves[index.row()];
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
			Leaf leaf(tokens[0], tokens[1]);
			leaf.type = Leaf::String;
			QStringList values;
			foreach(QString s, mdata[tokens[0]]) {
				if(s == "type:boolean") {
					leaf.type = Leaf::Enum;
					leaf.values << "true" << "false";
					break;
				} else if(s == "type:unsignedInt") {
					leaf.type = Leaf::Number;
					leaf.min  = 0;
					leaf.max  = INT_MAX;
					leaf.step = 1;
				} else if(s == "type:int") {
					leaf.type = Leaf::Number;
					leaf.min  = INT_MIN;
					leaf.max  = INT_MAX;
					leaf.step = 1;
				} else if(s.startsWith("range:")) {
					QStringList r = s.split(':');
					if(r.size() >= 3) {
						leaf.step = r[2].toInt();
						r = r[1].split("..");
						if(r.size() >= 2) {
							if(r[0] == "*")
								leaf.min = INT_MIN;
							else
								leaf.min = r[0].toInt();
							if(r[1] == "*")
								leaf.max = INT_MIN;
							else
								leaf.max = r[1].toInt();
						}
					}
				} else if(s.startsWith("units:")) {
					leaf.units = s.mid(s.indexOf(':') + 1);
				} else if(s == "type:dateTime") {
					leaf.type = Leaf::Date;
					break;
				} else if(s.startsWith("type:string")) {
					leaf.type = Leaf::String;
					leaf.max = INT_MAX;
					QStringList r = s.split("..");
					if(r.size() > 1)
						leaf.max = r[1].toInt();
				} else if(s.startsWith("list:")) {
					leaf.type = Leaf::String;
					leaf.max = INT_MAX;
					break;
				} else if(s.startsWith("enum:")) {
					leaf.type = Leaf::Enum;
					leaf.values << s.mid(s.indexOf(':') + 1);
				} else if(s.startsWith("pathref:targetParent:")) {
					leaf.type = Leaf::Enum;

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
					leaf.values << client->objects.filter(QRegExp(ref));
				}
			}
			leaves << leaf;
		}
	}
	if(!leaves.isEmpty())
		enumeration = leaves[0].name.count('.') > path.count('.') + 1;

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

QVariant Leaves::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role != Qt::DisplayRole)
		return QVariant::Invalid;

	if(orientation == Qt::Horizontal) {
		if(!section)
			return QString("Parameter");
		else
			return QString("Value");
	} else if(orientation == Qt::Vertical) {
		if(enumeration) {
			QString name = leaves[section].name;
			name = name.mid(name.lastIndexOf('.') - 1);
			return name.mid(0, name.indexOf('.'));
		}
	}

	return QVariant::Invalid;
}

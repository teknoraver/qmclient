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

#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QDateTimeEdit>

#include "leaf.h"
#include "mddelegate.h"

QWidget* MetaDataDelegate::createEditor(QWidget *parent,
					const QStyleOptionViewItem &,
					const QModelIndex &index) const
{
	Leaf *leaf = (Leaf*)index.internalPointer();

	switch(leaf->type) {
	case Leaf::Number: {
		QSpinBox *editor = new QSpinBox(parent);
		editor->setRange(INT_MIN, INT_MAX);
		return editor;
	}
	case Leaf::String: {
		QLineEdit *editor = new QLineEdit(parent);
		return editor;
	}
	case Leaf::Enum: {
		QComboBox *editor = new QComboBox(parent);
		return editor;
	}
	case Leaf::Date: {
		QDateTimeEdit *editor = new QDateTimeEdit(parent);
		editor->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
		editor->setCalendarPopup(true);
		return editor;
	}
	}

	return NULL;
}

void MetaDataDelegate::setEditorData(QWidget *edit, const QModelIndex &index) const
{
	Leaf *leaf = (Leaf*)index.internalPointer();

	switch(leaf->type) {
	case Leaf::Number: {
		QSpinBox *editor = static_cast<QSpinBox *>(edit);
		editor->setValue(leaf->value.toInt());
		break;
	}
	case Leaf::String: {
		QLineEdit *editor = static_cast<QLineEdit *>(edit);
		editor->setText(leaf->value);
		break;
	}
	case Leaf::Enum: {
		QComboBox *editor = static_cast<QComboBox *>(edit);
		editor->addItems(leaf->values);
		break;
	}
	case Leaf::Date: {
		QDateTimeEdit *editor = static_cast<QDateTimeEdit *>(edit);
		editor->setDateTime(QDateTime::fromString(leaf->value, Qt::ISODate));
		break;
	}
	}
}

void MetaDataDelegate::setModelData(QWidget *edit, QAbstractItemModel *model,
				   const QModelIndex &index) const
{
	Leaf *leaf = (Leaf*)index.internalPointer();
	QVariant value;

	switch(leaf->type) {
	case Leaf::Number: {
		QSpinBox *editor = static_cast<QSpinBox *>(edit);
		editor->interpretText();
		value = editor->value();
		break;
	}
	case Leaf::String: {
		QLineEdit *editor = static_cast<QLineEdit *>(edit);
		value = editor->text();
		break;
	}
	case Leaf::Enum: {
		QComboBox *editor = static_cast<QComboBox *>(edit);
		value = editor->currentText();
		break;
	}
	case Leaf::Date: {
		QDateTimeEdit *editor = static_cast<QDateTimeEdit *>(edit);
		value = editor->dateTime();
		break;
	}
	}
	model->setData(index, value, Qt::EditRole);
}

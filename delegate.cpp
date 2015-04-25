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

#include "leave.h"
#include "delegate.h"

QWidget *MetaDataDelegate::createEditor(QWidget *parent,
				       const QStyleOptionViewItem &,
				       const QModelIndex &index) const
{
	Leave *leave = (Leave*)index.internalPointer();

	switch(leave->type) {
	case Leave::ReadOnly:
		return NULL;
	case Leave::Number: {
		QSpinBox *editor = new QSpinBox(parent);
		editor->setRange(INT_MIN, INT_MAX);
		return editor;
	}
	case Leave::String: {
		QLineEdit *editor = new QLineEdit(parent);
		return editor;
	}
	case Leave::Enum: {
		QComboBox *editor = new QComboBox(parent);
		return editor;
	}
	case Leave::Date: {
		QDateTimeEdit *editor = new QDateTimeEdit(parent);
		return editor;
	}
	}

	return NULL;
}

void MetaDataDelegate::setEditorData(QWidget *edit, const QModelIndex &index) const
{
	Leave *leave = (Leave*)index.internalPointer();

	switch(leave->type) {
	case Leave::ReadOnly:
		return;
	case Leave::Number: {
		QSpinBox *editor = static_cast<QSpinBox *>(edit);
		editor->setValue(leave->value.toInt());
		break;
	}
	case Leave::String: {
		QLineEdit *editor = static_cast<QLineEdit *>(edit);
		editor->setText(leave->value);
		break;
	}
	case Leave::Enum: {
		QComboBox *editor = static_cast<QComboBox *>(edit);
		editor->addItems(leave->values);
		break;
	}
	case Leave::Date: {
		QDateTimeEdit *editor = static_cast<QDateTimeEdit *>(edit);
		editor->setDateTime(QDateTime::fromString(leave->value, Qt::ISODate));
		break;
	}
	}
}

void MetaDataDelegate::setModelData(QWidget *edit, QAbstractItemModel *model,
				   const QModelIndex &index) const
{
	Leave *leave = (Leave*)index.internalPointer();
	QVariant value;

	switch(leave->type) {
	case Leave::ReadOnly:
		return;
	case Leave::Number: {
		QSpinBox *editor = static_cast<QSpinBox *>(edit);
		editor->interpretText();
		value = editor->value();
		break;
	}
	case Leave::String: {
		QLineEdit *editor = static_cast<QLineEdit *>(edit);
		value = editor->text();
		break;
	}
	case Leave::Enum: {
		QComboBox *editor = static_cast<QComboBox *>(edit);
		value = editor->currentText();
		break;
	}
	case Leave::Date: {
		QDateTimeEdit *editor = static_cast<QDateTimeEdit *>(edit);
		value = editor->dateTime();
		break;
	}
	}
	model->setData(index, value, Qt::EditRole);
}

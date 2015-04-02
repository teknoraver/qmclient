#ifndef LEAVES_H
#define LEAVES_H

class Leaves : public QAbstractTableModel
{
public:
	void fillData(QStringList &data);

private:
	int rowCount(const QModelIndex&) const;
	int columnCount(const QModelIndex&) const;
	QVariant data(const QModelIndex&, int) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;

	QStringList leaves;
	QStringList values;
};

#endif

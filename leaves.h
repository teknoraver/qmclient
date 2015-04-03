#ifndef LEAVES_H
#define LEAVES_H

#include <QTcpSocket>
#include <QStringList>

class Leaves : public QAbstractTableModel
{
public:
	Leaves();
	void setSocket(QTcpSocket *s);
	void fillData(QStringList &data);

private:
	int rowCount(const QModelIndex&) const;
	int columnCount(const QModelIndex&) const;
	QVariant data(const QModelIndex&, int) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
	QTcpSocket *sock;

	QStringList leaves;
	QStringList values;
};

#endif

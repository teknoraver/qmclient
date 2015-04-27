#ifndef LEAF_H
#define LEAF_H

class Leaf
{
public:
	enum Type {
		String,
		Number,
		Date,
		Enum
	};

	Leaf(const QString &n, const QString &v, Type t, QStringList va) :
				name(n), value(v), type(t), values(va) { }

	QString name;
	QString value;
	Type type;
	QStringList values;
};

#endif

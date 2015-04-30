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

	Leaf(const QString &n, const QString &v) : name(n), value(v) { }

	QString name;
	QString value;
	Type type;
	QStringList values;
	int min, max, step;
	QString units;
};

#endif

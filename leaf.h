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

	Leaf(const QString &n, const QString &v) :
		name(n),
		value(v),
		type(String),
		range(false) { }

	QString name;
	QString value;
	Type type;
	QStringList values;
	bool sign;
	bool range;
	int min, max, step;
	QString units;
};

#endif

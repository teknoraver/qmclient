#ifndef LEAF_H
#define LEAF_H

#include <climits>

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
		sign(false),
		range(false),
		min(0),
		max(INT_MAX),
		step(1) { }

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

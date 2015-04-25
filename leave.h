#ifndef LEAVE_H
#define LEAVE_H

class Leave
{
public:
	enum Type {
		ReadOnly,
		Number,
		String,
		Date,
		Enum
	};

	Leave(const QString &n, const QString &v, Type t, QStringList va) :
				name(n), value(v), type(t), values(va) { }

	QString name;
	QString value;
	Type type;
	QStringList values;
};

#endif

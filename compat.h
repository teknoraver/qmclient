#ifndef COMPAT_H
#define COMPAT_H

#if QT_MAJOR_VERSION >= 5
#	define setClearButton(x) x->setClearButtonEnabled(true)
#else
#	define setClearButton(x)
#	define Q_DECL_OVERRIDE
#endif

#endif

#ifndef COMPAT_H
#define COMPAT_H

#if QT_VERSION >= 0x050200
#	define setClearButton(x) x->setClearButtonEnabled(true)
#else
#	define setClearButton(x)
#endif

#if QT_VERSION < 0x050000
#	define Q_DECL_OVERRIDE
#endif

#endif

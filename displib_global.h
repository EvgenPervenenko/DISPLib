#ifndef DISPLIB_GLOBAL_H
#define DISPLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DISPLIB_LIBRARY)
#  define DISPLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DISPLIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // TESTLIB_GLOBAL_H

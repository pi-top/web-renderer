#include "logger.h"

// "Static" instances of these variables, used with extern
// elsewhere to reference these instances.

QList<int64_t> g_threadIdTable;
int g_loggingMode = None;
int g_loggingLevel = LOG_DEBUG;

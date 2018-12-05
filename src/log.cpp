#include <lal/log.h>
#include "bolt/bolt.h"
#include "log.hpp"

logger(log_debug)
logger(log_info)
logger(log_warn)
logger(log_error)

struct BoltLog
*create_logger() {
	struct BoltLog *log = BoltLog_create(0);
	BoltLog_set_debug_func(log, _log_debug);
	BoltLog_set_warning_func(log, _log_warn);
	BoltLog_set_info_func(log, _log_info);
	BoltLog_set_error_func(log, _log_error);
	return log;
}


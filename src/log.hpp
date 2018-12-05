#ifndef _LAL_SEABOLT_LOG_H
#define _LAL_SEABOLT_LOG_H


#define logger(logger) void _##logger (void *state, const char *msg){ (void)state; logger(msg); }

struct BoltLog *create_logger();

#endif // _LAL_SEABOLT_LOG_H


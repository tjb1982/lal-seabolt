#ifndef _MODELS_COMMON_HPP_x
#define _MODELS_COMMON_HPP_x

#include <string>
#include "bolt/bolt.h"

#define MAX_FIELD_LENGTH (1<<8)
#define UUID_LENGTH (37)

using namespace std;


time_t
deserialize_time(const char *src);

void
serialize_time(time_t, char *buf);

void
set_created_param(BoltConnection *connection, int idx);


void
set_kv_param(BoltConnection *connection, int idx, const char *key, string value);


int
check_and_print_error(BoltConnection* connection, BoltStatus* status, const char* error_text);

#endif // _MODELS_COMMON_HPP

#include <string.h>
#include <lal/log.h>
#include <string>
#include "bolt/bolt.h"

#include "common.hpp"

using namespace std;

#define ISO_8601_FMT "%FT%TZ"

time_t
deserialize_time(const char *src)
{
	struct tm t;
	strptime(src, ISO_8601_FMT, &t);
	return mktime(&t);
}


void
serialize_time(const time_t time, char *buf)
{
	const struct tm *t = gmtime(&time);
	strftime(buf, MAX_FIELD_LENGTH - 1, ISO_8601_FMT, t);
}

void
set_created_param(BoltConnection *connection, int idx)
{
	char outstr[MAX_FIELD_LENGTH];
	time_t now = time(0);
	serialize_time(now, outstr);
	BoltValue *created_value = BoltConnection_set_run_cypher_parameter(
		connection, idx, "created", 7);
	BoltValue_format_as_String(created_value, outstr, strlen(outstr));
}


void
set_kv_param(BoltConnection *connection, int idx, const char *key, string value)
{
	BoltValue *bolt_value = BoltConnection_set_run_cypher_parameter(
		connection, idx, key, strlen(key));
	BoltValue_format_as_String(bolt_value, value.c_str(), value.length());
}


int
check_and_print_error(BoltConnection* connection, BoltStatus* status, const char* error_text)
{
	int error_code = BoltStatus_get_error(status);
	if (error_code==BOLT_SUCCESS) {
		return BOLT_SUCCESS;
	}

	if (error_code == BOLT_SERVER_FAILURE) {
		char string_buffer[4096];
		if (
			BoltValue_to_string(
				BoltConnection_failure(connection),
				string_buffer,
				4096,
				connection
			) > 4096
		) {
			string_buffer[4095] = 0;
		}
		log_error(
			"%s: %s", 
			error_text == NULL ? "server failure" : error_text,
			string_buffer
		);
	}
	else {
		const char* error_ctx = BoltStatus_get_error_context(status);
		log_error(
			"%s (code: %04X, text: %s, context: %s)\n",
			error_text==NULL ? "Bolt failure" : error_text,
			error_code,
			BoltError_get_string(error_code),
			error_ctx
		);
	}
	return error_code;
}

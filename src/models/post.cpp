#include <string.h>
#include <uuid/uuid.h>
#include <vector>
#include <lal/log.h>
#include "bolt/bolt.h"
#include "json.hpp"

#include "common.hpp"
#include "post.hpp"

using namespace std;
using json = nlohmann::json;


static const char *fields[4] = {
	"id",
	"title",
	"body",
	"created"
};


int
_run_request(BoltConnection *connection, BoltRequest& run, BoltRequest& pull) {
	int count = 0;

	if (BoltConnection_load_run_request(connection)) {
		return check_and_print_error(
			connection, BoltConnection_status(connection), "load run request failed"
		);
	}
	run = BoltConnection_last_request(connection);
	if (BoltConnection_load_pull_request(connection, -1)) {
		return check_and_print_error(
			connection, BoltConnection_status(connection), "load pull request failed"
		);
	}
	pull = BoltConnection_last_request(connection);

	if (BoltConnection_send(connection)) {
		return check_and_print_error(
			connection, BoltConnection_status(connection), "send failed"
		);
	}

	count = BoltConnection_fetch_summary(connection, run);
	if (count < 0 || !BoltConnection_summary_success(connection)) {
		return check_and_print_error(
			connection, BoltConnection_status(connection), "fetch summary failed"
		);
	}
	return 0;
}


int
model_insert_post(BoltConnector *connector, json& j, Post& created)
{
	int i, err = 0, count;
	char post_id_str[UUID_LENGTH];
	uuid_t post_id;
	const char *cypher =
		"create (post:Post{"
			"id: $id, "
			"title: $title, "
			"body: $body, "
			"created: datetime($created)"
		"}) return post.id, post.title, post.body, toString(post.created)";
	BoltRequest run, pull;
	BoltValue *value;

	// get connection or return 500
	BoltStatus *status = BoltStatus_create();
	BoltConnection *connection = BoltConnector_acquire(
		connector, BOLT_ACCESS_MODE_READ, status
	);

	if ((err = check_and_print_error(NULL, status, "unable to acquire connection"))) {
		goto exit;
	}

	log_trace("Cypher query: %s", cypher);

	if (BoltConnection_set_run_cypher(connection, cypher, strlen(cypher), 4)) {
		err = check_and_print_error(
			connection, BoltConnection_status(connection), "cypher execution failed"
		);
		goto exit;
	}

	uuid_generate(post_id);
	uuid_unparse(post_id, post_id_str);

	// set params
	set_kv_param(connection, 0, "id", string(post_id_str));
	set_kv_param(connection, 1, "title", j["title"]);
	set_kv_param(connection, 2, "body", j["body"]);
	set_created_param(connection, 3);

	if ((err = _run_request(connection, run, pull))) {
		goto exit;
	}

	while (BoltConnection_fetch(connection, pull) > 0) {
		const struct BoltValue *field_values = BoltConnection_field_values(connection);
		char id[37];
		value = BoltList_value(field_values, 0);
		BoltValue_to_string(value, id, UUID_LENGTH, connection);

		char title[MAX_FIELD_LENGTH];
		value = BoltList_value(field_values, 1);
		BoltValue_to_string(value, title, MAX_FIELD_LENGTH - 1, connection);

		char body[MAX_FIELD_LENGTH];
		value = BoltList_value(field_values, 2);
		BoltValue_to_string(value, body, MAX_FIELD_LENGTH - 1, connection);

		char created_string[MAX_FIELD_LENGTH];
		value = BoltList_value(field_values, 3);
		BoltValue_to_string(value, created_string, MAX_FIELD_LENGTH - 1, connection);
		time_t time_created = deserialize_time(created_string);

		created = Post(id, title, body, time_created);
	}
	err = check_and_print_error(
		connection, BoltConnection_status(connection), "query execution failed"
	);
exit:
	BoltStatus_destroy(status);
	BoltConnector_release(connector, connection);
	return err;
}

int
model_get_all_posts(BoltConnector *connector, vector<Post>& post_vector)
{
	int i = 0, count = 0;
	int err = BOLT_SUCCESS;
	const char *cypher =
		"match (post:Post) "
		"return post.id, post.title, post.body, toString(post.created)";
	BoltRequest run, pull;

	// need to check the status on every call that might change it (the way they do)
	BoltStatus *status = BoltStatus_create();
	BoltConnection *connection = BoltConnector_acquire(
		connector, BOLT_ACCESS_MODE_READ, status
	);

	if ((err = check_and_print_error(NULL, status, "unable to acquire connection"))) {
		goto exit;
	}

	log_trace("Cypher query: %s", cypher);

	if (BoltConnection_set_run_cypher(connection, cypher, strlen(cypher), 0)) {
		err = check_and_print_error(
			connection, BoltConnection_status(connection), "cypher execution failed"
		);
		goto exit;
	}

	if ((err = _run_request(connection, run, pull))) {
		goto exit;
	}

	while (BoltConnection_fetch(connection, pull) > 0) {
		const struct BoltValue *field_values = BoltConnection_field_values(connection);
		struct BoltValue *value;

		char id[37];
		value = BoltList_value(field_values, 0);
		BoltValue_to_string(value, id, UUID_LENGTH, connection);

		char title[MAX_FIELD_LENGTH];
		value = BoltList_value(field_values, 1);
		BoltValue_to_string(value, title, MAX_FIELD_LENGTH - 1, connection);

		char body[MAX_FIELD_LENGTH];
		value = BoltList_value(field_values, 2);
		BoltValue_to_string(value, body, MAX_FIELD_LENGTH - 1, connection);

		char created_string[MAX_FIELD_LENGTH];
		value = BoltList_value(field_values, 3);
		BoltValue_to_string(value, created_string, MAX_FIELD_LENGTH - 1, connection);
		time_t time_created = deserialize_time(created_string);

		Post p(id, string(title), string(body), time_created);
		post_vector.push_back(p);
	}
	err = check_and_print_error(
		connection, BoltConnection_status(connection), "query execution failed"
	);
exit:
	BoltStatus_destroy(status);
	BoltConnector_release(connector, connection);
	return err;
}

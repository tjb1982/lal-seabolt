#include <sstream>
#include <lal/request.h>
#include <lal/response.h>

#include "bolt/bolt.h"
#include "json.hpp"

#include "common.hpp"
#include "post.hpp"
#include "../models/common.hpp"
#include "../models/post.hpp"


using namespace std;
using json = nlohmann::json;


int
respond_with_posts(int sock, std::vector<Post> &posts) {
	int i = 0;
	auto ss = std::stringstream();
	char post_id[UUID_LENGTH];

	ss << "[";

	for (; i < posts.size(); i++) {
		Post p = posts[i];

		uuid_unparse(p.id, post_id);

		char created[MAX_FIELD_LENGTH];
		serialize_time(p.created, created);
		ss	<< "{"
				<< "\"title\":\"" << p.title << "\","
				<< "\"body\":\"" << p.body << "\","
				<< "\"id\":\"" << post_id << "\","
				<< "\"created\":\"" << created << "\""
			<< "}" << (i + 1 != posts.size() ? "," : "");
	}

	ss << "]";

	struct lal_response *resp = lal_create_response("200 OK");
	lal_append_to_entries(resp->headers,
		"Content-Type", "application/json");

	lal_nappend_to_body(resp->body,
		(const uint8_t *)ss.str().c_str(), ss.str().size());

	return respond(sock, resp);
}


int
respond_with_post(int sock, const char *status, Post& p)
{
	// create response 201
	struct lal_response *resp = lal_create_response(status);
	lal_append_to_entries(resp->headers,
		"Content-Type", "application/json");

	stringstream ss;

	char post_id[UUID_LENGTH];
	uuid_unparse(p.id, post_id);

	char created[MAX_FIELD_LENGTH];
	serialize_time(p.created, created);
	ss	<< "{"
			<< "\"title\":\"" << p.title << "\","
			<< "\"body\":\"" << p.body << "\","
			<< "\"id\":\"" << post_id << "\","
			<< "\"created\":\"" << created << "\""
		<< "}";

	auto dump = ss.str();
	lal_nappend_to_body(resp->body,
		(const uint8_t *)dump.c_str(), dump.length());

	return respond(sock, resp);
}

int
route_insert_post(int sock, struct lal_request *request) {

	int content_length, err = 0;
	json insert;
	Post created;

	// assert facts about headers and possibly respond if error.
	if ((err = common_middleware(sock, request))) { return err; }

	// assert post-specific facts and set content-length
	if ((err = post_middleware(sock, request, &content_length))) { return err; }

	// recv content_length from sock into buffer
	char buffer[content_length + 1];
	recv(sock, buffer, content_length, 0);
	buffer[content_length] = '\0';

	// parse json
	insert = json::parse(buffer);
	log_info("JSON received: %s", insert.dump().c_str());

	BoltConnector *connector = (BoltConnector *) request->extra;
	if (!connector) {
		const char *msg = "No database connector found.";
		log_error(msg);
		return respond_500(sock, msg);
	}

	if ((err = model_insert_post(connector, insert, created))) {
		return respond_500(sock, "Could not insert post.");
	}

	return respond_with_post(sock, "201 Created", created);
}


int
route_get_all_posts (int sock, struct lal_request *request) {
	int err = 0;
	vector<Post> post_vector = vector<Post>();
	BoltConnector *connector = (BoltConnector *) request->extra;
	if (!connector) {
		const char *msg = "No database connector found.";
		log_error(msg);
		return respond_500(sock, msg);
	}

	if ((err = common_middleware(sock, request))) { return err; }

	if ((err = model_get_all_posts(connector, post_vector))) {
		// bad
		return respond_500(sock, "Failed to fetch posts.");
	}

	return respond_with_posts(sock, post_vector);
}

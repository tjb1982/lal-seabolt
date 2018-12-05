#include <lal/response.h>

#include "json.hpp"
#include "common.hpp"


using json = nlohmann::json;


int
respond(int sock, struct lal_response *resp) {
	long long len;
	uint8_t *response = (uint8_t*)lal_serialize_response(resp, &len);
	send(sock, response, len, 0);
	free(response);
	lal_destroy_response(resp);
	return 0;
}


int
respond_500 (int sock, const char *msg) {
	struct lal_response *resp = lal_create_response("500 Bad");
	json j = {
		{"message", msg}
	};
	std::string dump = j.dump();
	lal_nappend_to_body(resp->body,
		(const uint8_t *)dump.c_str(), dump.size());
	respond(sock, resp);
	return 1;
}


int
post_middleware(int sock, struct lal_request *request, int *clptr) {

	struct lal_entry *header;

	// Get Content-Type header, value == application/json
	if (!(header = lal_get_header(request, "Content-Type"))) {
		return respond_500(sock, "Content-Type header was empty");
	}
	char content_type_str[header->vallen + 1];
	strncpy(content_type_str, header->val, header->vallen);
	if (strstr(content_type_str, "application/json") != content_type_str) {
		return respond_500(sock, "Content-Type must be application/json");
	}

	// Get Content-Length header, value
	if (!(header = lal_get_header(request, "Content-Length"))) {
		return respond_500(sock, "Content-Length header was empty");
	}
	char content_header_str[header->vallen + 1];
	strncpy(content_header_str, header->val, header->vallen);

	// Convert content_length to int
	int content_length = std::stoi(content_header_str);
	log_trace("Content length is: %i", content_length);
	*clptr = content_length;

	if (content_length > MAX_POST_CONTENT_LENGTH) {
		const char *format = "Content-Length must be less than or equal to %li bytes.";
		char msg[strlen(format) + 2];
		snprintf(msg, strlen(format) + 100, format, MAX_POST_CONTENT_LENGTH);
		respond_500(sock, msg);
		return 1;
	}

	return 0;
}


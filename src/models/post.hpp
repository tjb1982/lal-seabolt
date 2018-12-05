#ifndef _MODELS_POST_HPP
#define _MODELS_POST_HPP

#include <uuid/uuid.h>
#include <string>
#include "bolt/bolt.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;


struct Post {
public:
	uuid_t id;
	std::string title;
	std::string body;
	time_t created;

	Post(
		std::string id_str,
		std::string title,
		std::string body,
		time_t created
	) : title(title), body(body), created(created) {
		uuid_parse(id_str.c_str(), id);
	}
	Post() {}
};

int
model_get_all_posts(BoltConnector *, vector<Post>&);

int
model_insert_post(BoltConnector *, json& j, Post& created);

#endif // _MODELS_POST_HPP

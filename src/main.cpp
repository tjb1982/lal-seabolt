#include <stdio.h>
#include <time.h>
#include <lal/route.h>
#include <lal/request.h>
#include <lal/response.h>
#include <lal/network.h>
#include <lal/log.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "bolt/bolt.h"
#include "bolt/lifecycle.h"
#include "bolt/values.h"

#include "log.hpp"
#include "routes/common.hpp"
#include "routes/post.hpp"

#define QUOTE(...) #__VA_ARGS__

using namespace std;

const char *
getenv_or_default(const char *key, const char *default_value) {
	const char *val = getenv(key);
	return !val ? default_value : val;
}

BoltConnector *
db_up(int argc, char **argv) {
	const char *username = getenv_or_default("DB_USERNAME", "neo4j");
	const char *password = getenv_or_default("DB_PASSWORD", "password");

	Bolt_startup();

	BoltLog *log = create_logger();
	BoltConfig *config = BoltConfig_create();
	BoltConfig_set_mode(config, BOLT_MODE_DIRECT);
	BoltConfig_set_transport(config, BOLT_TRANSPORT_PLAINTEXT);
	BoltConfig_set_user_agent(config, "seabolt/" SEABOLT_VERSION);
	BoltConfig_set_max_pool_size(config, 1<<4);
	BoltConfig_set_log(config, log);

	struct BoltValue *auth_token = BoltAuth_basic(username, password, NULL);
	struct BoltAddress *address = BoltAddress_create((char*) "localhost", (char*) "7687");

	BoltConnector *connector = BoltConnector_create(address, auth_token, config);

	BoltValue_destroy(auth_token);
	BoltAddress_destroy(address);
	BoltLog_destroy(log);
	BoltConfig_destroy(config);

	return connector;
}


void
db_down(BoltConnector *connector) {
	BoltConnector_destroy(connector);
	Bolt_shutdown();
}


int
main(int argc, char **argv)
{
	struct lal_route *routes;
	const char *host = --argc ? *argv++ : "localhost";
	const char *port = argc-- ? *argv++ : "5000";

	setenv("TZ", "UTC", 1);

	BoltConnector *connector = db_up(argc, argv);

	routes = lal_init_routes();
	lal_register_route(routes, GET, "/posts/", route_get_all_posts, (void *)connector);
	lal_register_route(routes, POST, "/posts/", route_insert_post, (void *)connector);
	//lal_register_route(routes, GET, "/posts/:id", route_get_post_by_id, (void *)connector);

	lal_serve_forever(host, port, lal_route_request, routes);

	db_down(connector);

	lal_destroy_routes(routes);

	return 0;
}

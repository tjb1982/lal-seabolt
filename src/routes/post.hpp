#ifndef _ROUTES_POST_HPP
#define _ROUTES_POST_HPP

#include <vector>
#include <lal/request.h>
#include "../models/post.hpp"


int
respond_with_posts(int sock, std::vector<Post> &posts);

int
route_insert_post(int sock, struct lal_request *request);

int
route_get_all_posts (int sock, struct lal_request *request);

#endif // _ROUTES_POST_HPP

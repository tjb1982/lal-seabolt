#ifndef _ROUTES_COMMON_HPP
#define _ROUTES_COMMON_HPP

#define MAX_POST_CONTENT_LENGTH 1 << 12


int
respond_500 (int sock, const char *msg);

int
respond(int sock, struct lal_response *resp);

int
common_middleware(int sock, struct lal_request *request);

int
post_middleware(int sock, struct lal_request *request, int *clptr);


#endif // _ROUTES_COMMON_HPP

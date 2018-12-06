#include <stdlib.h>
#include "utils.hpp"

const char *
getenv_or_default(const char *key, const char *default_value)
{
	const char *val = getenv(key);
	return !val ? default_value : val;
}

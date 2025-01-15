#ifndef _UTILS_HPP
#define _UTILS_HPP

static char* concat(const char* s1, const char* s2) {
	char* result = new char[strlen(s1) + strlen(s2) + 1];

	size_t len1 = strlen(s1);
	size_t len2 = strlen(s2);

	memcpy(result, s1, len1);
	memcpy(result + len1, s2, len2 + 1);

	return result;
} // concat

#endif // _UTILS_HPP
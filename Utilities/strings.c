#include <Utilities/strings.h>

void strLwr(char *str)
{
	for (; *str; ++str)
		*str = tolower(*str);
}

void strUpr(char *str)
{
	for (; *str; ++str)
		*str = toupper(*str);
}
#include <Platform.h>
#ifdef UNIX
void DisplayError()
{
	int theError = errno;
}
#endif

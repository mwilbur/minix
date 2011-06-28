#include <stdio.h>

int main(void)
{
#ifdef __SIZEOF_LONG_DOUBLE__
	printf("%d\n", __SIZEOF_LONG_DOUBLE__);
	return 0;
#else
	printf("%d\n", sizeof(long double));
	return 0;
#endif
}

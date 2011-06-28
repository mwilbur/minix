#include <machine/asm.h>

.data
IMPORT(endbss)
ENTRY(_brksize)
.long	endbss

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <errno.h>
#include <psl1ght/types.h>

u32 crc32buf(char *buf, size_t len);
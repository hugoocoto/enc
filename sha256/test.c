#include "sha256.h"
#include "../base64/base64.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void
test(char *buf)
{
        int fd = mkstemp(strdup("/tmp/temp_file_XXXXXX"));
        int fd2 = mkstemp(strdup("/tmp/temp_file_XXXXXX"));
        write(fd, buf, strlen(buf));
        lseek(fd, 0, SEEK_SET);
        sha256(fd, strlen(buf), fd2);
        lseek(fd2, 0, SEEK_SET);
        uint32_t n[8];
        size_t size;
        read(fd2, &n, 8 * sizeof(uint32_t));
        char *s = b64_enc(&n, sizeof(uint32_t) * 8, &size);
        printf("%s ==> %s\n", buf, s);
        close(fd);
        close(fd2);
}

int
main()
{
        test("a");
        test("b");
        test("Hello, World!");
}

#define INCLUDE_B64_IMPLEMENTATION
#include "../base64/base64.h"

#define INCLUDE_SHA256_IMPLEMENTATION
#include "sha256.h"

#include "sha256.h"
#include "../base64/base64.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
test(char *buf, const char *expected_b64)
{
        char tmp1[] = "/tmp/temp_file_XXXXXX";
        char tmp2[] = "/tmp/temp_file_XXXXXX";
        int fd = mkstemp(tmp1);
        int fd2 = mkstemp(tmp2);
        if (fd < 0 || fd2 < 0) {
                fprintf(stderr, "mkstemp failed\n");
                if (fd >= 0) close(fd);
                if (fd2 >= 0) close(fd2);
                return 0;
        }
        write(fd, buf, strlen(buf));
        lseek(fd, 0, SEEK_SET);
        sha256(fd, strlen(buf), fd2);
        lseek(fd2, 0, SEEK_SET);
        uint32_t n[8];
        size_t size;
        read(fd2, &n, 8 * sizeof(uint32_t));
        char *s = b64_enc(&n, sizeof(uint32_t) * 8, &size);
        int pass = strcmp(s, expected_b64) == 0;
        printf("%s ==> %s [%s]\n", buf, s, pass ? "PASS" : "FAIL");
        if (!pass)
                printf("  expected: %s\n", expected_b64);
        free(s);
        close(fd);
        close(fd2);
        return pass;
}

int
main()
{
        int passed = 0;
        int total = 3;
        passed += test("a",             "ypeBEsobvcr6wjGzmiPcTaeG7/gUfE5yuYB3ha/uSLs=");
        passed += test("b",             "PiPoFgA5WUoziU9lZOGxNIu9egCI1CxKy3PurtWcAJ0=");
        passed += test("Hello, World!", "3/1gIbsr1bCvZ2KQgJ7DpTGR3YHH9wpLKGiKNiGCmG8=");
        printf("Summary: [%d/%d] tests passed\n", passed, total);
        return passed == total ? 0 : 1;
}

#define INCLUDE_B64_IMPLEMENTATION
#include "../base64/base64.h"

#define INCLUDE_SHA256_IMPLEMENTATION
#include "sha256.h"

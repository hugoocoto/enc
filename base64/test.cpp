#include "base64.h"

#include <stdio.h>
#include <string.h>

int passed = 0;
int total = 0;

void
test(void *data, size_t size, void *data_expected, size_t size_expected)
{
        size_t new_size;
        char *res = b64_enc(data, size, &new_size);

        ++total;

        if (new_size != size_expected) {
                printf("Test failed: size unmatch at enc\n");
                return;
        }

        if (memcmp(res, data_expected, size_expected)) {
                printf("Test failed: memory unmatch at enc: (%s) got, but (%s) expected\n", res, (char *) data_expected);
                return;
        }

        ++passed;
        ++total;

        size_t new_size2;
        char *res2 = (char *) b64_dec(res, new_size, &new_size2);

        if (new_size2 != size) {
                printf("Test failed: size unmatch at dec\n");
                return;
        }

        if (memcmp(data, res2, size)) {
                printf("Test failed: memory unmatch at dec: (%s) got, but (%s) expected\n", res2, (char *) data);
                return;
        }

        ++passed;
}

#define test(x, y) test((void *) x, strlen(x), (void *) y, strlen(y))

int
main()
{
        test("", "");
        test("M", "TQ==");
        test("Ma", "TWE=");
        test("Man", "TWFu");
        test("Manchester", "TWFuY2hlc3Rlcg==");
        printf("Summary: [%d/%d] tests passed\n", passed, total);
}

#define INCLUDE_B64_IMPLEMENTATION
#include "base64.h"

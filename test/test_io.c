/*
 * This file is part of libsdlog.
 *
 * Copyright 2023-2024 Tamas Nepusz.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <sdlog/streams.h>
#include <stdio.h>

#include "config.h"

#include "unity.h"
#include "utils.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_ostream_file(void)
{
#if HAVE_FMEMOPEN
    char buf[32];
    unsigned char template[] = "12345678901234567890";
    FILE* fp = fmemopen(buf, sizeof(buf), "wb");
    sdlog_ostream_t stream;

    TEST_ASSERT_NOT_NULL(fp);
    TEST_CHECK(sdlog_ostream_init_file(&stream, fp));

    TEST_CHECK(sdlog_ostream_write_all(&stream, template, 20));
    TEST_CHECK(sdlog_ostream_flush(&stream));
    TEST_ERROR(SDLOG_EWRITE, sdlog_ostream_write_all(&stream, template, 20));
    TEST_ASSERT_EQUAL_STRING_LEN("12345678901234567890123456789012", buf, 32);

    sdlog_ostream_destroy(&stream);

    fclose(fp);
#else
    TEST_IGNORE()
#endif
}

void test_ostream_null(void)
{
    unsigned char template[] = "12345678901234567890";
    sdlog_ostream_t stream;

    TEST_CHECK(sdlog_ostream_init_null(&stream));
    TEST_CHECK(sdlog_ostream_write_all(&stream, template, 20));
    TEST_CHECK(sdlog_ostream_flush(&stream));
    sdlog_ostream_destroy(&stream);
}

void test_ostream_buffer(void)
{
    unsigned char template[] = "12345678901234567890";
    sdlog_ostream_t stream;
    const uint8_t* buf;
    size_t length;

    TEST_CHECK(sdlog_ostream_init_buffer(&stream));

    /* To test zero-length writes */
    TEST_CHECK(sdlog_ostream_write(&stream, template, 0, &length));
    TEST_ASSERT_EQUAL(0, length);
    TEST_CHECK(sdlog_ostream_write(&stream, template, 0, NULL));

    TEST_CHECK(sdlog_ostream_write_all(&stream, template, 20));
    TEST_CHECK(sdlog_ostream_flush(&stream));
    TEST_CHECK(sdlog_ostream_write_all(&stream, template, 20));
    TEST_CHECK(sdlog_ostream_write_all(&stream, template, 20));

    buf = sdlog_ostream_buffer_get(&stream, &length);
    TEST_ASSERT_EQUAL(60, length);
    TEST_ASSERT_EQUAL_STRING_LEN(
        "123456789012345678901234567890123456789012345678901234567890",
        buf, length);

    sdlog_ostream_destroy(&stream);
}

int main(int argc, char* argv[])
{
    UNITY_BEGIN();

    RUN_TEST(test_ostream_file);
    RUN_TEST(test_ostream_null);
    RUN_TEST(test_ostream_buffer);

    return UNITY_END();
}

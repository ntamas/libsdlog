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

#include <sdlog/encoder.h>
#include <sdlog/writer.h>
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

void test_writer_init_destroy(void)
{
    sdlog_writer_t writer;
    sdlog_ostream_t stream;

    TEST_CHECK(sdlog_ostream_init_null(&stream));
    TEST_CHECK(sdlog_writer_init(&writer, &stream));

    sdlog_writer_destroy(&writer);
    sdlog_ostream_destroy(&stream);
}

void test_writer_formats(void)
{
    sdlog_writer_t writer;
    sdlog_ostream_t stream;
    sdlog_message_format_t int_format;
    sdlog_message_format_t float_format;
    const uint8_t* buf;
    /* clang-format off */
    uint8_t expected[] = {
        0xA3, 0x95, 0x80, 0x01, 30,
        'I', 'N', 'T', 0,
        'b', 'B', 'h', 'H', 'i', 'I', 'q', 'Q', 0, 0, 0, 0, 0, 0, 0, 0,
        's', '8', ',', 'u', '8', ',', 's', '1', '6', ',', 'u', '1', '6', ',',
        's', '3', '2', ',', 'u', '3', '2', ',', 's', '6', '4', ',', 'u', '6', '4',
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0,

        0xA3, 0x95, 0x01, 0xfe, 0xef, 0xfe, 0xca, 0xef, 0xbe,
        0xfe, 0xca, 0xad, 0x0b, 0xef, 0xbe, 0xad, 0xde,
        0xfe, 0xca, 0xad, 0x0b, 0x00, 0x00, 0x00, 0x00,
        0xef, 0xbe, 0xad, 0xde, 0x00, 0x00, 0x00, 0x00,

        0xA3, 0x95, 0x80, 0x02, 12,
        'F', 'L', 'T', 0,
        'f', 'd', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        'f', 'l', 'o', 'a', 't', ',', 'd', 'o', 'u', 'b', 'l', 'e',
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,

        0xA3, 0x95, 0x02,
        0x00, 0x00, 0x00, 0x3e,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd0, 0x3f,
    };
    /* clang-format on */
    size_t size;

    TEST_CHECK(sdlog_ostream_init_buffer(&stream));

    TEST_CHECK(sdlog_message_format_init(&int_format, 1, "INT"));
    TEST_CHECK(sdlog_message_format_add_columns(
        &int_format,
        "s8,u8,s16,u16,s32,u32,s64,u64",
        "bBhHiIqQ",
        "--------"));

    TEST_CHECK(sdlog_message_format_init(&float_format, 2, "FLT"));
    TEST_CHECK(sdlog_message_format_add_columns(
        &float_format,
        "float,double",
        "fd",
        "--"));

    TEST_CHECK(sdlog_writer_init(&writer, &stream));

    TEST_CHECK(sdlog_writer_write(
        &writer, &int_format,
        0x0badcafe, 0xdeadbeef, 0x0badcafe, 0xdeadbeef,
        0x0badcafe, 0xdeadbeef, 0x0badcafeLL, 0xdeadbeefULL));

    TEST_CHECK(sdlog_writer_write(&writer, &float_format, 0.125, 0.25));

    sdlog_writer_destroy(&writer);

    sdlog_message_format_destroy(&float_format);
    sdlog_message_format_destroy(&int_format);

    buf = sdlog_ostream_buffer_get(&stream, &size);
    TEST_ASSERT_NOT_NULL(buf);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, buf, sizeof(expected));
    TEST_ASSERT_EQUAL(sizeof(expected), size);

    sdlog_ostream_destroy(&stream);
}

void test_writer_write_encoded(void)
{
    sdlog_writer_t writer;
    sdlog_ostream_t stream;
    sdlog_message_format_t int_format;
    uint8_t encoded[128];
    size_t length;
    const uint8_t* buf;
    /* clang-format off */
    uint8_t expected[] = {
        0xA3, 0x95, 0x80, 0x01, 30,
        'I', 'N', 'T', 0,
        'b', 'B', 'h', 'H', 'i', 'I', 'q', 'Q', 0, 0, 0, 0, 0, 0, 0, 0,
        's', '8', ',', 'u', '8', ',', 's', '1', '6', ',', 'u', '1', '6', ',',
        's', '3', '2', ',', 'u', '3', '2', ',', 's', '6', '4', ',', 'u', '6', '4',
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0,

        0xA3, 0x95, 0x01, 0xfe, 0xef, 0xfe, 0xca, 0xef, 0xbe,
        0xfe, 0xca, 0xad, 0x0b, 0xef, 0xbe, 0xad, 0xde,
        0xfe, 0xca, 0xad, 0x0b, 0x00, 0x00, 0x00, 0x00,
        0xef, 0xbe, 0xad, 0xde, 0x00, 0x00, 0x00, 0x00,

        0xA3, 0x95, 0x01, 0xfe, 0xef, 0xfe, 0xca, 0xef, 0xbe,
        0xfe, 0xca, 0xad, 0x0b, 0xef, 0xbe, 0xad, 0xde,
        0xfe, 0xca, 0xad, 0x0b, 0x00, 0x00, 0x00, 0x00,
        0xef, 0xbe, 0xad, 0xde, 0x00, 0x00, 0x00, 0x00,
    };
    /* clang-format on */
    size_t size;

    TEST_CHECK(sdlog_ostream_init_buffer(&stream));

    TEST_CHECK(sdlog_message_format_init(&int_format, 1, "INT"));
    TEST_CHECK(sdlog_message_format_add_columns(
        &int_format,
        "s8,u8,s16,u16,s32,u32,s64,u64",
        "bBhHiIqQ",
        "--------"));

    TEST_CHECK(sdlog_message_format_encode(&int_format, encoded, &length,
        0x0badcafe, 0xdeadbeef, 0x0badcafe, 0xdeadbeef,
        0x0badcafe, 0xdeadbeef, 0x0badcafeLL, 0xdeadbeefULL));

    TEST_CHECK(sdlog_writer_init(&writer, &stream));
    TEST_CHECK(sdlog_writer_write_encoded(&writer, &int_format, encoded, length));
    TEST_CHECK(sdlog_writer_write_encoded(&writer, &int_format, encoded, 0));
    sdlog_writer_destroy(&writer);

    sdlog_message_format_destroy(&int_format);

    buf = sdlog_ostream_buffer_get(&stream, &size);
    TEST_ASSERT_NOT_NULL(buf);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, buf, sizeof(expected));
    TEST_ASSERT_EQUAL(sizeof(expected), size);

    sdlog_ostream_destroy(&stream);
}

int main(int argc, char* argv[])
{
    UNITY_BEGIN();

    RUN_TEST(test_writer_init_destroy);
    RUN_TEST(test_writer_formats);
    RUN_TEST(test_writer_write_encoded);

    return UNITY_END();
}

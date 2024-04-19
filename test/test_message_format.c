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
#include <sdlog/model.h>
#include <stdlib.h>

#include "unity.h"
#include "utils.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void check_fmt_column(const sdlog_message_format_t* format)
{
    const sdlog_message_column_format_t* col;
    char* fmt;

    TEST_ASSERT_EQUAL(SDLOG_ID_FMT, sdlog_message_format_get_id(format));
    TEST_ASSERT_EQUAL_STRING("FMT", sdlog_message_format_get_type(format));

    TEST_ASSERT_EQUAL(5, sdlog_message_format_get_column_count(format));

    TEST_ASSERT_NOT_NULL(col = sdlog_message_format_get_column(format, 0));
    TEST_ASSERT_EQUAL('B', col->type);
    TEST_ASSERT_EQUAL('-', col->unit);
    TEST_ASSERT_EQUAL_STRING("Type", col->name);

    TEST_ASSERT_NOT_NULL(col = sdlog_message_format_get_column(format, 2));
    TEST_ASSERT_EQUAL('n', col->type);
    TEST_ASSERT_EQUAL('-', col->unit);
    TEST_ASSERT_EQUAL_STRING("Name", col->name);

    TEST_ASSERT_NULL(col = sdlog_message_format_get_column(format, 5));

    fmt = sdlog_message_format_get_format_string(format);
    TEST_ASSERT_NOT_NULL(fmt);
    TEST_ASSERT_EQUAL_STRING("BBnNZ", fmt);
    free(fmt);

    fmt = sdlog_message_format_get_column_names(format, ", ");
    TEST_ASSERT_NOT_NULL(fmt);
    TEST_ASSERT_EQUAL_STRING("Type, Length, Name, Format, Columns", fmt);
    free(fmt);

    TEST_ASSERT_EQUAL(86, sdlog_message_format_get_size(format));
}

void test_create_empty_message_format(void)
{
    sdlog_message_format_t format;

    TEST_CHECK(sdlog_message_format_init(&format, SDLOG_ID_FMT, "FMT"));
    TEST_ASSERT_EQUAL(SDLOG_ID_FMT, sdlog_message_format_get_id(&format));
    TEST_ASSERT_EQUAL_STRING("FMT", sdlog_message_format_get_type(&format));
    TEST_ASSERT_EQUAL(0, sdlog_message_format_get_column_count(&format));
    TEST_ASSERT_EQUAL(0, sdlog_message_format_get_size(&format));

    sdlog_message_format_destroy(&format);
}

void test_create_message_format_with_columns(void)
{
    sdlog_message_format_t format;

    TEST_CHECK(sdlog_message_format_init(&format, SDLOG_ID_FMT, "FMT"));
    TEST_CHECK(sdlog_message_format_add_column(&format, "Type", 'B', '-'));
    TEST_CHECK(sdlog_message_format_add_column(&format, "Length", 'B', '-'));
    TEST_CHECK(sdlog_message_format_add_column(&format, "Name", 'n', '-'));
    TEST_CHECK(sdlog_message_format_add_column(&format, "Format", 'N', '-'));
    TEST_CHECK(sdlog_message_format_add_column(&format, "Columns", 'Z', '-'));

    check_fmt_column(&format);

    sdlog_message_format_destroy(&format);
}

void test_create_message_format_with_columns_convenience(void)
{
    sdlog_message_format_t format;

    TEST_CHECK(sdlog_message_format_init(&format, SDLOG_ID_FMT, "FMT"));
    TEST_CHECK(sdlog_message_format_add_columns(
        &format, "Type,Length,Name,Format,Columns", "BBnNZ", "-----"));

    check_fmt_column(&format);

    sdlog_message_format_destroy(&format);
}

void test_invalid_message_format_type(void)
{
    sdlog_message_format_t format;
    TEST_ERROR(SDLOG_EINVAL, sdlog_message_format_init(&format, 127, "FOOBAR"));
}

void test_invalid_message_column_type(void)
{
    sdlog_message_format_t format;
    TEST_CHECK(sdlog_message_format_init(&format, SDLOG_ID_FMT, "FMT"));
    TEST_ERROR(SDLOG_EINVAL, sdlog_message_format_add_column(&format, "Type", '@', '-'));
    sdlog_message_format_destroy(&format);
}

void test_message_encoding(void)
{
    sdlog_message_format_t format;
    uint8_t buf[SDLOG_MAX_MESSAGE_LENGTH];
    size_t written;

    TEST_CHECK(sdlog_message_format_init(&format, SDLOG_ID_FMT, "FMT"));
    TEST_CHECK(sdlog_message_format_add_column(&format, "Type", 'B', '-'));
    TEST_CHECK(sdlog_message_format_add_column(&format, "Length", 'B', '-'));
    TEST_CHECK(sdlog_message_format_add_column(&format, "Name", 'n', '-'));
    TEST_CHECK(sdlog_message_format_add_column(&format, "Format", 'N', '-'));
    TEST_CHECK(sdlog_message_format_add_column(&format, "Columns", 'Z', '-'));

    TEST_CHECK(sdlog_message_format_encode(&format, buf, &written, 42, 8, "FOO", "Id", "B"));
    TEST_ASSERT_EQUAL(sdlog_message_format_get_size(&format) + 3, written);

    sdlog_message_format_destroy(&format);
}

void test_message_encoding_invalid_format_code(void)
{
    sdlog_message_format_t format;
    uint8_t buf[SDLOG_MAX_MESSAGE_LENGTH];

    TEST_CHECK(sdlog_message_format_init(&format, SDLOG_ID_FMT, "FMT"));
    TEST_CHECK(sdlog_message_format_add_column(&format, "Type", 'a', '-'));
    TEST_ERROR(
        sdlog_message_format_encode(&format, buf, NULL, 42),
        SDLOG_UNIMPLEMENTED);
    sdlog_message_format_destroy(&format);
}

int main(int argc, char* argv[])
{
    UNITY_BEGIN();

    RUN_TEST(test_create_empty_message_format);
    RUN_TEST(test_invalid_message_format_type);
    RUN_TEST(test_invalid_message_column_type);
    RUN_TEST(test_create_message_format_with_columns);
    RUN_TEST(test_create_message_format_with_columns_convenience);
    RUN_TEST(test_message_encoding);
    RUN_TEST(test_message_encoding_invalid_format_code);

    return UNITY_END();
}

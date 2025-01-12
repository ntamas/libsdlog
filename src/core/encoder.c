/*
 * This file is part of libsdlog.
 *
 * Copyright 2023-2025 Tamas Nepusz.
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

#include <string.h>

#include <sdlog/encoder.h>
#include <sdlog/error.h>

#include "endianness.h"

sdlog_error_t sdlog_message_format_encode(
    const sdlog_message_format_t* format, uint8_t* buf, size_t* written, ...)
{
    va_list args;
    sdlog_error_t retval;

    va_start(args, written);
    retval = sdlog_message_format_encode_va(format, buf, written, args);
    va_end(args);

    return retval;
}

sdlog_error_t sdlog_message_format_encode_va(
    const sdlog_message_format_t* format, uint8_t* buf, size_t* written, va_list args)
{
    uint8_t i, num_columns = sdlog_message_format_get_column_count(format);
    uint8_t* write_ptr = buf;
    union {
        float as_float;
        double as_double;
        uint32_t as_uint32;
        uint64_t as_uint64;
    } value;

    *(write_ptr++) = 0xA3;
    *(write_ptr++) = 0x95;
    *(write_ptr++) = format->id;

    for (i = 0; i < num_columns; i++) {
        const sdlog_message_column_format_t* column = sdlog_message_format_get_column(format, i);
        char type = column->type;
        uint8_t num_bytes;

        switch (type) {
        case 'b': /* int8_t */
        case 'B': /* uint8_t */
        case 'M': /* flight mode as uint8_t */
            *write_ptr = va_arg(args, int);
            write_ptr++;
            break;

        case 'c': /* fixed-point float as signed short int, multiplier: 0.01 */
        case 'C': /* fixed-point float as unsigned short int, multiplier: 0.01 */
        case 'h': /* int16_t */
        case 'H': /* uint16_t */
            store16_to_LE(va_arg(args, int), write_ptr);
            write_ptr += 2;
            break;

        case 'e': /* fixed-point float as signed int, multiplier: 0.01 */
        case 'E': /* fixed-point float as unsigned int, multiplier: 0.01 */
        case 'L': /* fixed-point float as signed int, multiplier: 1e-7, for geodetic coordinates */
        case 'i': /* int32_t */
        case 'I': /* uint32_t */
            store32_to_LE(va_arg(args, int32_t), write_ptr);
            write_ptr += 4;
            break;

        case 'q': /* int64_t */
        case 'Q': /* uint64_t */
            store64_to_LE(va_arg(args, int64_t), write_ptr);
            write_ptr += 8;
            break;

        case 'f': /* IEEE single-precision float */
            value.as_float = va_arg(args, double);
            store32_to_LE(value.as_uint32, write_ptr);
            write_ptr += 4;
            break;

        case 'd': /* IEEE double-precision float */
            value.as_double = va_arg(args, double);
            store64_to_LE(value.as_uint64, write_ptr);
            write_ptr += 8;
            break;

        case 'n': /* string, max length: 4 */
        case 'N': /* string, max length: 16 */
        case 'Z': /* string, max length: 64 */
            num_bytes = (type == 'Z' ? 64 : (type == 'N' ? 16 : 4));
            memset(write_ptr, 0, num_bytes);
            strncpy((char*)write_ptr, va_arg(args, char*), num_bytes);
            write_ptr += num_bytes;
            break;

        case 'a': /* int16_t[32] */
            return SDLOG_UNIMPLEMENTED;

        default: /* unknown */
            return SDLOG_EINVAL; /* LCOV_EXCL_LINE */
        }
    }

    if (written) {
        *written = write_ptr - buf;
    }

    return SDLOG_SUCCESS;
}

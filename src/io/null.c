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

#include <stdlib.h>

#include <sdlog/streams.h>

#include "stream_base.h"

static sdlog_error_t null_write(
    sdlog_ostream_t* stream, uint8_t* data, size_t length, size_t* written);

const sdlog_ostream_spec_t sdlog_ostream_null_methods = {
    .write = null_write,
};

sdlog_error_t sdlog_ostream_init_null(sdlog_ostream_t* stream)
{
    return sdlog_ostream_init(stream, &sdlog_ostream_null_methods, NULL);
}

static sdlog_error_t null_write(
    sdlog_ostream_t* stream, uint8_t* data, size_t length, size_t* written)
{
    *written = length;
    return SDLOG_SUCCESS;
}

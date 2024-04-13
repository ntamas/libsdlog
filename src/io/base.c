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

#include <assert.h>
#include <string.h>

#include <sdlog/streams.h>

sdlog_error_t sdlog_ostream_init(
    sdlog_ostream_t* stream, const sdlog_ostream_spec_t* spec,
    void* ctx)
{
    assert(spec);

    memset(stream, 0, sizeof(sdlog_ostream_t));
    stream->methods = spec;
    stream->context = ctx;

    return SDLOG_SUCCESS;
}

void sdlog_ostream_destroy(sdlog_ostream_t* stream)
{
    if (stream->methods->destroy) {
        stream->methods->destroy(stream);
    }

    memset(stream, 0, sizeof(sdlog_ostream_t));
}

sdlog_error_t sdlog_ostream_begin_session(sdlog_ostream_t* stream)
{
    return stream->methods->begin
        ? stream->methods->begin(stream)
        : SDLOG_SUCCESS;
}

sdlog_error_t sdlog_ostream_end_session(sdlog_ostream_t* stream)
{
    return stream->methods->end
        ? stream->methods->end(stream)
        : SDLOG_SUCCESS;
}

sdlog_error_t sdlog_ostream_flush(sdlog_ostream_t* stream)
{
    return stream->methods->flush
        ? stream->methods->flush(stream)
        : SDLOG_SUCCESS;
}

sdlog_error_t sdlog_ostream_write(
    sdlog_ostream_t* stream, uint8_t* data, size_t length, size_t* bytes_written)
{
    size_t dummy;

    if (length == 0) {
        if (bytes_written) {
            *bytes_written = 0;
        }
        return SDLOG_SUCCESS;
    }

    return stream->methods->write
        ? stream->methods->write(stream, data, length, bytes_written ? bytes_written : &dummy)
        : SDLOG_UNIMPLEMENTED;
}

sdlog_error_t sdlog_ostream_write_all(
    sdlog_ostream_t* stream, uint8_t* data, size_t length)
{
    uint8_t* buf = data;
    size_t written;

    while (length > 0) {
        SDLOG_CHECK(sdlog_ostream_write(stream, buf, length, &written));
        if (written <= length) {
            length -= written;
            buf += written;
        } else {
            /* Should not happen */
            /* LCOV_EXCL_START */
            return SDLOG_EWRITE;
            /* LCOV_EXCL_STOP */
        }
    }

    return SDLOG_SUCCESS;
}

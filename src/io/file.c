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

typedef struct {
    FILE* fp;
} context_t;

static void file_destroy(sdlog_ostream_t* stream);
static sdlog_error_t file_flush(sdlog_ostream_t* stream);
static sdlog_error_t file_write(sdlog_ostream_t* stream, uint8_t* data, size_t length);

const sdlog_ostream_spec_t sdlog_ostream_file_methods = {
    .destroy = file_destroy,
    .flush = file_flush,
    .write = file_write,
};

sdlog_error_t sdlog_ostream_init_file(sdlog_ostream_t* stream, FILE* fp)
{
    context_t* ctx;

    SDLOG_CHECK_OOM(ctx = calloc(1, sizeof(context_t)));
    ctx->fp = fp;

    return sdlog_ostream_init(stream, &sdlog_ostream_file_methods, ctx);
}

static void file_destroy(sdlog_ostream_t* stream)
{
    context_t* ctx = CONTEXT_AS(context_t);
    ctx->fp = NULL;
    free(ctx);
}

static sdlog_error_t file_flush(sdlog_ostream_t* stream)
{
    context_t* ctx = CONTEXT_AS(context_t);
    return fflush(ctx->fp) ? SDLOG_EWRITE : SDLOG_SUCCESS;
}

static sdlog_error_t file_write(sdlog_ostream_t* stream, uint8_t* data, size_t length)
{
    context_t* ctx = CONTEXT_AS(context_t);
    return fwrite(data, sizeof(uint8_t), length, ctx->fp) == length ? SDLOG_SUCCESS : SDLOG_EWRITE;
}
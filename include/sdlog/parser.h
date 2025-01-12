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

#ifndef SDLOG_PARSER_H
#define SDLOG_PARSER_H

#include <sdlog/streams.h>

/**
 * @file parser.h
 * @brief Log parser structure that parses logs from an abstract stream
 */

__BEGIN_DECLS

typedef struct {
    /** The output stream that the parser parses the log from */
    sdlog_istream_t* stream;
} sdlog_parser_t;

/**
 * @brief Creates a new log parser that parses the log from the given stream.
 *
 * @param parser  the parser to initialize
 * @param stream  the stream to read the log from
 */
sdlog_error_t sdlog_parser_init(sdlog_parser_t* parser, sdlog_istream_t* stream);

/**
 * @brief Destroys the log parser.
 *
 * @param writer  the parser to destroy
 */
void sdlog_parser_destroy(sdlog_parser_t* parser);

__END_DECLS

#endif

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

/**
 * \file version.h
 * \brief Version information for \c libsdlog
 */

#ifndef SDLOG_VERSION_H
#define SDLOG_VERSION_H

/**
 * \def SDLOG_VERSION_MAJOR
 * Major version number of \c libsdlog
 */
#define SDLOG_VERSION_MAJOR 0

/**
 * \def SDLOG_VERSION_MINOR
 * Minor version number of \c libsdlog
 */
#define SDLOG_VERSION_MINOR 3

/**
 * \def SDLOG_VERSION_PATCH
 * Patch level of \c libsdlog
 */
#define SDLOG_VERSION_PATCH 0

/**
 * \def SDLOG_VERSION
 * Unified version number of \c libsdlog
 */
#define SDLOG_VERSION (SDLOG_VERSION_MAJOR * 10000 + SDLOG_VERSION_MINOR * 100 + SDLOG_VERSION_PATCH)

#endif

/*
 * This file is part of libsdlog.
 *
 * Copyright 2023 Tamas Nepusz.
 *
 * libsdlog is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * libsdlog is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SDLOG_ERROR_H
#define SDLOG_ERROR_H

#include <sdlog/decls.h>

/**
 * @file error.h
 * @brief Error codes and error handling related functions
 *
 * @def SDLOG_CHECK(func)
 * @brief Executes the given function and checks its return code.
 *
 * This macro should be used in functions that return an \ref sdlog_error_t
 * error code. The function argument is also expected to return an \ref sdlog_error_t
 * error code. Any return value from the function that is not equal to
 * \ref SDLOG_SUCCESS is returned intact to the caller.
 */

__BEGIN_DECLS

// clang-format off
/**
 * Error codes used throughout \c libsdlog.
 */
typedef enum {
    SDLOG_SUCCESS = 0,    /**< No error */
    SDLOG_FAILURE,        /**< Generic failure code */
    SDLOG_ENOMEM,         /**< Not enough memory */
    SDLOG_EINVAL,         /**< Invalid value */
} sdlog_error_t;
// clang-format on

#define SDLOG_CHECK(func)                      \
    {                                          \
        sdlog_error_t __sdlog_retval = (func); \
        if (__sdlog_retval != SB_SUCCESS) {    \
            return __sdlog_retval;             \
        }                                      \
    }

/**
 * Converts a \c libsdlog error code to a human-readable string.
 *
 * \return  a pointer to the string containing the error message. This string
 *          should not be modified under any circumstances.
 */
const char* sdlog_error_to_string(int code);

__END_DECLS

#endif

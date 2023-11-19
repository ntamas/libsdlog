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

#include <sdlog/error.h>

/* clang-format off */
static char *sdlog_i_error_messages[] = {
    "No error",                                            /* SDLOG_SUCCESS */
    "Unspecified failure",                                 /* SDLOG_FAILURE */
    "Not enough memory",                                   /* SDLOG_ENOMEM */
    "Invalid value"                                        /* SDLOG_EINVAL */
};
/* clang-format on */

const char* sdlog_error_to_string(int code)
{
    if (code >= 0 && code < (int)(sizeof(sdlog_i_error_messages) / sizeof(sdlog_i_error_messages[0])))
        return sdlog_i_error_messages[code];
    return sdlog_i_error_messages[SDLOG_FAILURE];
}

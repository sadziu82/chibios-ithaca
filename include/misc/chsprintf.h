/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    chprintf.h
 * @brief   Mini printf-like functionality.
 *
 * @addtogroup chprintf
 * @{
 */

#ifndef _CHSPRINTF_H_
#define _CHSPRINTF_H_

/**
 * @brief   Float type support.
 */
#if !defined(CHPRINTF_USE_FLOAT) || defined(__DOXYGEN__)
#define CHPRINTF_USE_FLOAT          TRUE
#endif

#ifdef __cplusplus
extern "C" {
#endif
  uint8_t chsprintf(char *, const char *, ...);
  uint8_t chvsprintf(char *, const char *, va_list);
#ifdef __cplusplus
}
#endif

#endif /* _CHSPRINTF_H_ */

/** @} */

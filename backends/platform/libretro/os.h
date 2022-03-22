/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef BACKENDS_LIBRETRO_OS_H
#define BACKENDS_LIBRETRO_OS_H

#define FORBIDDEN_SYMBOL_ALLOW_ALL
#include "libretro.h"

#include "base/main.h"
#include "common/system.h"
#include "graphics/surface.libretro.h"

#ifndef F_OK
#define F_OK 0
#endif

#ifndef W_OK
#define W_OK 2
#endif

#ifndef R_OK
#define R_OK 4
#endif

extern char cmd_params[20][200];
extern char cmd_params_num;

#if (defined(GEKKO) && !defined(WIIU)) || (defined(__PS3__) && !defined(__PSL1GHT__))
extern int access(const char *path, int amode);
#endif

OSystem* retroBuildOS(bool aEnableSpeedHack);
const Graphics::Surface& getScreen();

void retroProcessMouse(retro_input_state_t aCallback, int device, float gampad_cursor_speed, bool analog_response_is_quadratic, int analog_deadzone, float mouse_speed);
void retroPostQuit();

void retroSetSystemDir(const char* aPath);
void retroSetSaveDir(const char* aPath);

void retroKeyEvent(bool down, unsigned keycode, uint32_t character, uint16_t key_modifiers);

#endif

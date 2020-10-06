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

#include <common/system.h>
#include "engines/util.h"
#include "gfx.h"

namespace Scooby {

Gfx::Gfx(VDP *vdp): _vdp(vdp) {
	//TODO add support for more video modes like RGB565 and RGBA555
	_pixelFormat = Graphics::PixelFormat(4, 8, 8, 8, 8, 0, 8, 16, 24); // ABGR8888

	initGraphics(320, 240, &_pixelFormat);
	_backSurface = new Graphics::Surface();
	_backSurface->create(320, 240, _pixelFormat);
}

void Gfx::drawFrame() {
	uint8 *screen = (uint8 *)_backSurface->getPixels();
	int numScanLines = _vdp->num_scanlines();
	for (int sl = 0; sl < numScanLines; ++sl) {
		_vdp->scanline(screen);
		screen += _backSurface->pitch;
	}
	updateScreen();
}

void Gfx::updateScreen() {
	g_system->copyRectToScreen((byte*)_backSurface->getBasePtr(0, 0),
							   _backSurface->pitch,
							  0,0,
							  _backSurface->w, _backSurface->h);
	g_system->updateScreen();
}

} // End of namespace Scooby

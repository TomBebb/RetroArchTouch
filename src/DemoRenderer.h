/*
 * Copyright 2017 David Kozub <zub at linux.fjfi.cvut.cz>
 *
 * This file is part of MirGLESDemo.
 *
 * MirGLESDemo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MirGLESDemo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MirGLESDemo.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DEMO_RENDERER_H
#define DEMO_RENDERER_H

#include <mir_toolkit/events/event.h>

#include "MirNativeWindowRenderer.h"
#include "MirNativeWindowControl.h"

class DemoRenderer: public MirNativeWindowRenderer
{
public:
	DemoRenderer();
	virtual void run(MirNativeWindowControl& nativeWindow) override;
	virtual void handleEvent(const MirEvent* event) override;

private:
	void renderFrame();
	void handleInputEvent(const MirInputEvent* inputEvent);
	void handleInputTouchEvent(const MirTouchEvent* touchEvent);

	bool m_fingerDown;
};

#endif // DEMO_RENDERER_H

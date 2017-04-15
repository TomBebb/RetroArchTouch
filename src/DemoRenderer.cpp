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
#include "DemoRenderer.h"
#include "ResourcePath.h"
#include "PNGLoader.h"
#include "ShaderLoader.h"

#include <iostream>
#include <stdexcept>
#include <GLES2/gl2.h>

DemoRenderer::DemoRenderer():
	m_fingerDown(false)
{
	// TEST
	std::cout << "Loading image" << std::endl;
	Image image = loadPNG(getResourcePath("MirGLESDemo.png"));
	std::cout << "Loaded image: width = " << image.getWidth() << ", height = " << image.getHeight() << std::endl;
}

void DemoRenderer::run(MirNativeWindowControl& nativeWindow)
{
	std::cout << "Loading shader" << std::endl;
	std::shared_ptr<Shader> vertexShader = loadShader(ShaderType::Vertex, getResourcePath("vertex_shader.glslv"));
	std::shared_ptr<Shader> fragmentShader = loadShader(ShaderType::Fragment, getResourcePath("fragment_shader.glslf"));

	Program program(*vertexShader, *fragmentShader);
	program.bindAttribute(0, "vPosition");
	program.link();
	glUseProgram(program.getGLProgram());

	glViewport(0, 0, nativeWindow.getWidth(), nativeWindow.getHeight());
	glClearColor(0., 0., 1., 1.);

	const GLfloat vertices[] =
	{
		0.0f,  0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f,  0.0f
	};

	// Load the vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);

	for (int i = 0; i < 1000; i++)
	{
		renderFrame();
		nativeWindow.swapBuffers();
	}
}

void DemoRenderer::handleEvent(const MirEvent* event)
{
	const MirEventType type = mir_event_get_type(event);

	if (type == mir_event_type_input)
	{
		const MirInputEvent* inputEvent = mir_event_get_input_event(event);
		if (!inputEvent)
			throw std::runtime_error("mir_event_get_input_event returned null!");

		handleInputEvent(inputEvent);
	}
}

void DemoRenderer::renderFrame()
{
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 3);

#if 0
	if (m_fingerDown)
		glClearColor(1., 0., 0., 1.);
	else
		glClearColor(0., 0., 1., 1.);

	glClear(GL_COLOR_BUFFER_BIT);
#endif
}

void DemoRenderer::handleInputEvent(const MirInputEvent* inputEvent)
{
	const MirInputEventType inputType = mir_input_event_get_type(inputEvent);

	switch (inputType)
	{
	case mir_input_event_type_touch:
		{
			std::cout << "DemoRenderer::handleInputEvent: type TOUCH" << std::endl;
			MirTouchEvent const* touchEvent = mir_input_event_get_touch_event(inputEvent);
			if (!touchEvent)
				throw std::runtime_error("mir_input_event_get_touch_event returned null!");

			handleInputTouchEvent(touchEvent);
		}
		break;

	case mir_input_event_type_key:
		std::cout << "DemoRenderer::handleInputEvent: type KEY" << std::endl;
		break;

	case mir_input_event_type_pointer:
		std::cout << "DemoRenderer::handleInputEvent: type POINTER" << std::endl;
		break;
	}
}

void DemoRenderer::handleInputTouchEvent(const MirTouchEvent* touchEvent)
{
	const MirTouchAction action = mir_touch_event_action(touchEvent, 0);
	const float touchX = mir_touch_event_axis_value(touchEvent, 0, mir_touch_axis_x);
	const float touchY = mir_touch_event_axis_value(touchEvent, 0, mir_touch_axis_y);

	const char *actionStr;
	switch (action)
	{
	case mir_touch_action_up:
		actionStr = "up";
		break;
	case mir_touch_action_down:
		actionStr = "down";
		break;
	case mir_touch_action_change:
		actionStr = "change";
		break;
	}

	std::cout << "DemoRenderer::handleInputTouchEvent: finger#0 " << actionStr << " at (" << touchX << "," << touchY << ")" << std::endl;

	if (action == mir_touch_action_down)
		m_fingerDown = true;
	else if (action == mir_touch_action_up)
		m_fingerDown = false;
}

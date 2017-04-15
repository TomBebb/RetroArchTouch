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

#include "gl/Program.h"
#include "gl/ArrayBuffer.h"
#include "gl/Texture.h"

#include <iostream>
#include <stdexcept>
#include <cmath>
#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

DemoRenderer::DemoRenderer():
	m_mvpMatrixIndex(0),
	m_cubeVertexCount(0),
	m_fingerDown(false)
{}

template<typename VEC>
void addFace(std::vector<VEC>& triangles, const VEC& a, const VEC& b, const VEC& c, const VEC& d)
{
	// 1st triangle
	triangles.push_back(a);
	triangles.push_back(b);
	triangles.push_back(d);

	// 2nd triangle
	triangles.push_back(b);
	triangles.push_back(d);
	triangles.push_back(c);
}

void DemoRenderer::run(MirNativeWindowControl& nativeWindow)
{
	std::cout << "Loading shader" << std::endl;
	std::shared_ptr<Shader> vertexShader = loadShader(ShaderType::Vertex, getResourcePath("vertex_shader.glslv"));
	std::shared_ptr<Shader> fragmentShader = loadShader(ShaderType::Fragment, getResourcePath("fragment_shader.glslf"));

	Program program(*vertexShader, *fragmentShader);
	program.link();
	glUseProgram(program.getGLProgram());

	glViewport(0, 0, nativeWindow.getWidth(), nativeWindow.getHeight());
	glClearColor(0., 0., 0., 1.);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	const GLint vertexIndex = program.getAttribute("vPosition");
	const GLint colorIndex = program.getAttribute("vTexCoord");
	m_mvpMatrixIndex = program.getUniform("MVPMatrix");
	const GLint textureSamplerIndex = program.getUniform("textureSampler");

	std::vector<glm::vec3> v; // vertices
	std::vector<glm::vec2> c; // text coords
	v.reserve(6 /* faces */ * 2 /* 2 triangles for each face */);
	c.reserve(v.capacity());

	// z = +1
	addFace(v, glm::vec3(-1, -1, +1), glm::vec3(-1, +1, +1), glm::vec3(+1, +1, +1), glm::vec3(+1, -1, +1));
	addFace(c, glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0));

	// z = -1
	addFace(v, glm::vec3(-1, -1, -1), glm::vec3(-1, +1, -1), glm::vec3(+1, +1, -1), glm::vec3(+1, -1, -1));
	addFace(c, glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0));

	// x = +1
	addFace(v, glm::vec3(+1, -1, +1), glm::vec3(+1, +1, +1), glm::vec3(+1, +1, -1), glm::vec3(+1, -1, -1));
	addFace(c, glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0));

	// x = -1
	addFace(v, glm::vec3(-1, -1, +1), glm::vec3(-1, +1, +1), glm::vec3(-1, +1, -1), glm::vec3(-1, -1, -1));
	addFace(c, glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0));

	// y = +1
	addFace(v, glm::vec3(-1, +1, +1), glm::vec3(-1, +1, -1), glm::vec3(+1, +1, -1), glm::vec3(+1, +1, +1));
	addFace(c, glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0));

	// y = -1
	addFace(v, glm::vec3(-1, -1, +1), glm::vec3(-1, -1, -1), glm::vec3(+1, -1, -1), glm::vec3(+1, -1, +1));
	addFace(c, glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0));

	ArrayBuffer arrayBuffer;
	arrayBuffer.bind();
	glBufferData(GL_ARRAY_BUFFER, v.size()*sizeof(decltype(v[0])), glm::value_ptr(v[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(vertexIndex);
	glVertexAttribPointer(vertexIndex, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	ArrayBuffer colorBuffer;
	colorBuffer.bind();
	glBufferData(GL_ARRAY_BUFFER, c.size()*sizeof(decltype(c[0])), glm::value_ptr(c[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(colorIndex);
	glVertexAttribPointer(colorIndex, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	Texture2D texture(loadPNG(getResourcePath("MirGLESDemo.png")));
	texture.getGLTexture();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.getGLTexture());
	glUniform1i(textureSamplerIndex, 0 /* Texture unit 0 */);

	m_projectionMatrix = glm::perspective(glm::radians(45.0f),
										  static_cast<float>(nativeWindow.getWidth()) / static_cast<float>(nativeWindow.getHeight()),
										  0.1f, 100.0f);

	m_cubeVertexCount = v.size();

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view = glm::lookAt(
				glm::vec3(4,3,3),
				glm::vec3(0,0,0),
				glm::vec3(0,1,0)
				);

	static float angle = 0.0f;
	angle += 2.0*M_PI / 200;
	if (angle >= 2.0 * M_PI)
		angle -= 2.0 * M_PI;

	glm::mat4 model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 mvpMatrix = m_projectionMatrix * view * model;
	glUniformMatrix4fv(m_mvpMatrixIndex, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

	glDrawArrays(GL_TRIANGLES, 0, m_cubeVertexCount);

#if 0
	if (m_fingerDown)
		glClearColor(1., 0., 0., 1.);
	else
		glClearColor(0., 0., 1., 1.);
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

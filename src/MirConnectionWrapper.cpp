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
#include "Exceptions.h"
#include "MirConnectionWrapper.h"

MirConnectionWrapper::MirConnectionWrapper(const char* server, const char* appName):
	m_connection(mir_connect_sync(server, appName), mir_connection_release)
{
	if (!m_connection || !mir_connection_is_valid(m_connection.get()))
		throw MirConnectionError(m_connection.get());
}

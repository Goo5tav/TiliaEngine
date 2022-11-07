// Vendor
#include "vendor/glad/include/glad/glad.h"

// Standard
#include <vector>

// Tilia
#include "Core/Modules/Rendering/OpenGL/3.3/Abstractions/Shader_Part.hpp"
#include "Core/Modules/Rendering/OpenGL/3.3/Error_Handling.hpp"
#include "Core/Modules/Exceptions/Tilia_Exception.hpp"
#include "Core/Values/OpenGL/3.3/Utils.hpp"
#include "Core/Values/OpenGL/3.3/Enums.hpp"
#include "Core/Modules/File_System/Windows/File_System.hpp"

// The file system defined in another file
extern tilia::utils::File_System file_system;

void tilia::gfx::Shader_Part::Init()
{

	try {
		
		GL_CALL(m_ID = glCreateShader(*m_type));

	}
	catch(utils::Tilia_Exception& e) {
		e.Add_Message("Failed to init Shader_Part { ID: %v }")(m_ID);

		throw e;
	}

}

void tilia::gfx::Shader_Part::Source()
{

	try {
		m_source = file_system.Load_File(m_path);
	}
	catch(utils::Tilia_Exception& e)
	{
		e.Add_Message("Failed to load shader source for Shader_Part { ID: %v }")(m_ID);

		throw e;
	}

}

void tilia::gfx::Shader_Part::Compile()
{

	try {

		const char* src{ m_source.c_str() };

		GL_CALL(glShaderSource(m_ID, 1, &src, nullptr));

		GL_CALL(glCompileShader(m_ID));

		std::int32_t result;

		GL_CALL(glGetShaderiv(m_ID, GL_COMPILE_STATUS, &result));

		if (result == GL_FALSE) {
			std::int32_t length;

			GL_CALL(glGetShaderiv(m_ID, GL_INFO_LOG_LENGTH, &length));

			std::vector<char> message(static_cast<size_t>(length));

			GL_CALL(glGetShaderInfoLog(m_ID, length, &length, &message.front()));
			message[static_cast<size_t>(length) - 1] = '\0';

			GL_CALL(glDeleteShader(m_ID));

			utils::Tilia_Exception e{ LOCATION };

			e.Add_Message("Shader part { ID: %v } failed to be created"
				"\n>>> Part type: %v"
				"\n>>> Message: %v"
				)(m_ID)
				(utils::Get_Shader_Type_String(m_type))
				(&message.front());

			throw e;

		}

	}
	catch(utils::Tilia_Exception& e) {
		e.Add_Message("Failed to compile Shader_Part { ID: %v }")(m_ID);

		throw e;
	}

}

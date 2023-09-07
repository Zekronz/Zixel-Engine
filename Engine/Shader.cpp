/*
    Shader.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#include "Engine/ZixelPCH.h"
#include "Engine/Shader.h"
#include "Engine/File.h"

namespace Zixel {

	Shader::~Shader() {

		if (compiled) {

			ZIXEL_INFO("Destroyed shader \"{}\".", name);
			glDeleteProgram(shaderProgram);

		}

	}

	bool Shader::compile(std::string& vertexSource, std::string& fragmentSource, std::string& vertexPath, std::string& fragmentPath) {

		const char* vertSource = vertexSource.c_str();
		const char* fragSource = fragmentSource.c_str();

		s32 success;
		char infoLog[512];

		//Vertex shader.
		u32 vertShader = glCreateShader(GL_VERTEX_SHADER);
		if (vertShader == 0) {

			ZIXEL_CRITICAL("Unable to create vertex shader for \"{}\"", name);
			return false;

		}

		glShaderSource(vertShader, 1, &vertSource, NULL);
		glCompileShader(vertShader);

		glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
		if (!success) {

			glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
			ZIXEL_CRITICAL("Unable to compile vertex shader for \"{}\"\n{}", name, infoLog);
			glDeleteShader(vertShader);

			return false;

		}

		//Fragment shader.
		u32 fragShader = glCreateShader(GL_FRAGMENT_SHADER);
		if (fragShader == 0) {

			ZIXEL_CRITICAL("Unable to create fragment shader for \"{}\"", name);
			glDeleteShader(vertShader);

			return false;

		}

		glShaderSource(fragShader, 1, &fragSource, NULL);
		glCompileShader(fragShader);

		glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
		if (!success) {

			glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
			ZIXEL_CRITICAL("Unable to compile fragment shader for \"{}\"\n{}", name, infoLog);
			glDeleteShader(vertShader);
			glDeleteShader(fragShader);

			return false;

		}

		//Shader program.
		shaderProgram = glCreateProgram();
		if (shaderProgram == 0) {

			ZIXEL_CRITICAL("Error creating shader program for \"{}\"", name);
			glDeleteShader(vertShader);
			glDeleteShader(fragShader);

			return false;

		}

		glAttachShader(shaderProgram, vertShader);
		glAttachShader(shaderProgram, fragShader);
		glLinkProgram(shaderProgram);

		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {

			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			ZIXEL_CRITICAL("Unable to link shader for \"{}\"\n{}", name, infoLog);
			glDeleteProgram(shaderProgram);
			glDeleteShader(vertShader);
			glDeleteShader(fragShader);
			shaderProgram = 0;

			return false;

		}

		glDeleteShader(vertShader);
		glDeleteShader(fragShader);

		compiled = true;

		uniformMatModel = getUniformLocation("matModel");
		uniformMatProj = getUniformLocation("matProj");
		uniformQuadPos = getUniformLocation("quadPos");
		uniformAtlasUV = getUniformLocation("atlasUV");
		uniformHasTexture = getUniformLocation("hasTexture");
		uniformBlend = getUniformLocation("blend");

		ZIXEL_INFO("Loaded shader \"{}\".", name);

		return true;

	}

	bool Shader::loadFromFile(std::string vertexPath, std::string fragmentPath) {

		if (compiled) {

			ZIXEL_WARN("Shader \"{}\" has already been compiled.", name);
			return false;

		}

		name = File::getNameFromPath(vertexPath);

		std::string vertCode;
		std::string fragCode;

		std::ifstream vertShaderFile;
		std::ifstream fragShaderFile;

		vertShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {

			vertShaderFile.open(vertexPath.c_str());
			fragShaderFile.open(fragmentPath.c_str());

			std::stringstream vertShaderStream, fragShaderStream;

			vertShaderStream << vertShaderFile.rdbuf();
			fragShaderStream << fragShaderFile.rdbuf();

			vertShaderFile.close();
			fragShaderFile.close();

			vertCode = vertShaderStream.str();
			fragCode = fragShaderStream.str();

		}
		catch (std::ifstream::failure e) {

			ZIXEL_CRITICAL("Error reading shader file(s).\n\t{}\n\t{}", vertexPath, fragmentPath);
			return false;

		}

		return compile(vertCode, fragCode, vertexPath, fragmentPath);

	}

	s32 Shader::getUniformLocation(const char* name) {

		if (!compiled) return -1;
		return glGetUniformLocation(shaderProgram, name);

	}

	void Shader::setUniformBool(s32 location, bool value) {
		glUniform1i(location, value);
	}

	void Shader::setUniform1i(s32 location, s32 value) {
		glUniform1i(location, value);
	}

	void Shader::setUniform2i(s32 location, s32 value1, s32 value2) {
		glUniform2i(location, value1, value2);
	}

	void Shader::setUniform3i(s32 location, s32 value1, s32 value2, s32 value3) {
		glUniform3i(location, value1, value2, value3);
	}

	void Shader::setUniform4i(s32 location, s32 value1, s32 value2, s32 value3, s32 value4) {
		glUniform4i(location, value1, value2, value3, value4);
	}

	void Shader::setUniform1f(s32 location, f32 value1) {
		glUniform1f(location, value1);
	}

	void Shader::setUniform2f(s32 location, f32 value1, f32 value2) {
		glUniform2f(location, value1, value2);
	}

	void Shader::setUniform3f(s32 location, f32 value1, f32 value2, f32 value3) {
		glUniform3f(location, value1, value2, value3);
	}

	void Shader::setUniform4f(s32 location, f32 value1, f32 value2, f32 value3, f32 value4) {
		glUniform4f(location, value1, value2, value3, value4);
	}

	void Shader::setUniformMatrix4fv(s32 location, glm::mat4 matrix) {
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}
#pragma once

#include <string>
#include <unordered_map>
#include <GLM/glm.hpp>

namespace Zixel {

	struct Shader {

		u32 shaderProgram = 0;

		s32 uniformMatModel = -1;
		s32 uniformMatProj = -1;
		s32 uniformQuadPos = -1;
		s32 uniformAtlasUV = -1;
		s32 uniformHasTexture = -1;
		s32 uniformBlend = -1;

		std::string name = "";
		std::unordered_map<const char*, s32> uniformCache;
		bool compiled = false;

		bool compile(std::string& vertexSource, std::string& fragmentSource, std::string& vertexPath, std::string& fragmentPath);

		~Shader();

		bool loadFromFile(std::string vertexPath, std::string fragmentPath);

		s32 getUniformLocation(const char* name);
		void setUniformBool(s32 location, bool value);
		void setUniform1i(s32 location, s32 value);
		void setUniform2i(s32 location, s32 value1, s32 value2);
		void setUniform3i(s32 location, s32 value1, s32 value2, s32 value3);
		void setUniform4i(s32 location, s32 value1, s32 value2, s32 value3, s32 value4);
		void setUniform1f(s32 location, f32 value1);
		void setUniform2f(s32 location, f32 value1, f32 value2);
		void setUniform3f(s32 location, f32 value1, f32 value2, f32 value3);
		void setUniform4f(s32 location, f32 value1, f32 value2, f32 value3, f32 value4);
		void setUniformMatrix4fv(s32 location, glm::mat4 matrix);

	};

}
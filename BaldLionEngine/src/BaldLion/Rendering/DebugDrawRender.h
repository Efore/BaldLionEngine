#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#define DEBUG_DRAW_IMPLEMENTATION
#include "debug_draw.hpp"     

namespace BaldLion {

	namespace Rendering {

		class DebugDrawRender : public dd::RenderInterface
		{
		public:

			DebugDrawRender() 
			{
				
			}

			~DebugDrawRender() 
			{

			}

			void Init() 
			{
				// This has to be enabled since the point drawing shader will use gl_PointSize.
				glEnable(GL_PROGRAM_POINT_SIZE);

				setupShaderPrograms();
				setupVertexBuffers();
			}

			void Stop()
			{
				glDeleteProgram(m_linePointProgram);
				glDeleteProgram(m_textProgram);

				glDeleteVertexArrays(1, &m_linePointVAO);
				glDeleteBuffers(1, &m_linePointVBO);

				glDeleteVertexArrays(1, &m_textVAO);
				glDeleteBuffers(1, &m_textVBO);
			}

			void setMvpMatrix(const glm::mat4& mvpMatrix) 
			{
				m_mvpMatrix = mvpMatrix;
			}

			void drawPointList(const dd::DrawVertex * points, int count, bool depthEnabled) override 
			{
				assert(points != nullptr);
				assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

				glBindVertexArray(m_linePointVAO);
				glUseProgram(m_linePointProgram);

				glUniformMatrix4fv(m_linePointProgram_MvpMatrixLocation,
					1, GL_FALSE, (float*)(&m_mvpMatrix[0][0]));

				if (depthEnabled)
				{
					glEnable(GL_DEPTH_TEST);
				}
				else
				{
					glDisable(GL_DEPTH_TEST);
				}

				// NOTE: Could also use glBufferData to take advantage of the buffer orphaning trick...
				glBindBuffer(GL_ARRAY_BUFFER, m_linePointVBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), points);

				// Issue the draw call:
				glDrawArrays(GL_POINTS, 0, count);

				glUseProgram(0);
				glBindVertexArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				checkGLError(__FILE__, __LINE__);
			}

			void drawLineList(const dd::DrawVertex * lines, int count, bool depthEnabled) override 
			{
				assert(lines != nullptr);
				assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

				glBindVertexArray(m_linePointVAO);
				glUseProgram(m_linePointProgram);

				glUniformMatrix4fv(m_linePointProgram_MvpMatrixLocation,
					1, GL_FALSE, (float*)(&m_mvpMatrix[0][0]));

				if (depthEnabled)
				{
					glEnable(GL_DEPTH_TEST);
				}
				else
				{
					glDisable(GL_DEPTH_TEST);
				}

				// NOTE: Could also use glBufferData to take advantage of the buffer orphaning trick...
				glBindBuffer(GL_ARRAY_BUFFER, m_linePointVBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), lines);

				// Issue the draw call:
				glDrawArrays(GL_LINES, 0, count);

				glUseProgram(0);
				glBindVertexArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
								
				checkGLError(__FILE__, __LINE__);
			}

			void drawGlyphList(const dd::DrawVertex * glyphs, int count, dd::GlyphTextureHandle glyphTex) override 
			{
				assert(glyphs != nullptr);
				assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

				glBindVertexArray(m_textVAO);
				glUseProgram(m_textProgram);

				// These doesn't have to be reset every draw call, I'm just being lazy ;)
				glUniform1i(m_textProgram_GlyphTextureLocation, 0);
				glUniform2f(m_textProgram_ScreenDimensions,
					static_cast<GLfloat>(m_windowHeight),
					static_cast<GLfloat>(m_windowHeight));

				if (glyphTex != nullptr)
				{
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, handleToGL(glyphTex));
				}

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDisable(GL_DEPTH_TEST);

				glBindBuffer(GL_ARRAY_BUFFER, m_textVBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), glyphs);

				glDrawArrays(GL_TRIANGLES, 0, count); // Issue the draw call

				glDisable(GL_BLEND);
				glUseProgram(0);
				glBindVertexArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindTexture(GL_TEXTURE_2D, 0);

				checkGLError(__FILE__, __LINE__);
			}

			dd::GlyphTextureHandle createGlyphTexture(int width, int height, const void * pixels) override 
			{
				assert(width > 0 && height > 0);
				assert(pixels != nullptr);

				GLuint textureId = 0;
				glGenTextures(1, &textureId);
				glBindTexture(GL_TEXTURE_2D, textureId);

				glPixelStorei(GL_PACK_ALIGNMENT, 1);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

				glBindTexture(GL_TEXTURE_2D, 0);
				checkGLError(__FILE__, __LINE__);

				return GLToHandle(textureId);
			}

			void destroyGlyphTexture(dd::GlyphTextureHandle glyphTex) override 
			{
				if (glyphTex == nullptr)
				{
					return;
				}

				const GLuint textureId = handleToGL(glyphTex);
				glBindTexture(GL_TEXTURE_2D, 0);
				glDeleteTextures(1, &textureId);
			}

			void setupShaderPrograms() 
			{
				//
			// Line/point drawing shader:
			//
				{
					GLuint linePointVS = glCreateShader(GL_VERTEX_SHADER);
					glShaderSource(linePointVS, 1, &linePointVertShaderSrc, nullptr);
					compileShader(linePointVS);

					GLint linePointFS = glCreateShader(GL_FRAGMENT_SHADER);
					glShaderSource(linePointFS, 1, &linePointFragShaderSrc, nullptr);
					compileShader(linePointFS);

					m_linePointProgram = glCreateProgram();
					glAttachShader(m_linePointProgram, linePointVS);
					glAttachShader(m_linePointProgram, linePointFS);

					glBindAttribLocation(m_linePointProgram, 0, "in_Position");
					glBindAttribLocation(m_linePointProgram, 1, "in_ColorPointSize");
					linkProgram(m_linePointProgram);

					m_linePointProgram_MvpMatrixLocation = glGetUniformLocation(m_linePointProgram, "u_MvpMatrix");
				}

				//
				// Text rendering shader:
				//
				{
					GLuint textVS = glCreateShader(GL_VERTEX_SHADER);
					glShaderSource(textVS, 1, &textVertShaderSrc, nullptr);
					compileShader(textVS);

					GLint textFS = glCreateShader(GL_FRAGMENT_SHADER);
					glShaderSource(textFS, 1, &textFragShaderSrc, nullptr);
					compileShader(textFS);

					m_textProgram = glCreateProgram();
					glAttachShader(m_textProgram, textVS);
					glAttachShader(m_textProgram, textFS);

					glBindAttribLocation(m_textProgram, 0, "in_Position");
					glBindAttribLocation(m_textProgram, 1, "in_TexCoords");
					glBindAttribLocation(m_textProgram, 2, "in_Color");
					linkProgram(m_textProgram);

					m_textProgram_GlyphTextureLocation = glGetUniformLocation(m_textProgram, "u_glyphTexture");

					m_textProgram_ScreenDimensions = glGetUniformLocation(m_textProgram, "u_screenDimensions");

				}
			}

			void setupVertexBuffers() 
			{
				// Lines/points vertex buffer:
			//
				{
					glGenVertexArrays(1, &m_linePointVAO);
					glGenBuffers(1, &m_linePointVBO);
					checkGLError(__FILE__, __LINE__);

					glBindVertexArray(m_linePointVAO);
					glBindBuffer(GL_ARRAY_BUFFER, m_linePointVBO);

					// RenderInterface will never be called with a batch larger than
					// DEBUG_DRAW_VERTEX_BUFFER_SIZE vertexes, so we can allocate the same amount here.
					glBufferData(GL_ARRAY_BUFFER, DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex), nullptr, GL_STREAM_DRAW);
					checkGLError(__FILE__, __LINE__);

					// Set the vertex format expected by 3D points and lines:
					std::size_t offset = 0;

					glEnableVertexAttribArray(0); // in_Position (vec3)
					glVertexAttribPointer(
						/* index     = */ 0,
						/* size      = */ 3,
						/* type      = */ GL_FLOAT,
						/* normalize = */ GL_FALSE,
						/* stride    = */ sizeof(dd::DrawVertex),
						/* offset    = */ reinterpret_cast<void *>(offset));
					offset += sizeof(float) * 3;

					glEnableVertexAttribArray(1); // in_ColorPointSize (vec4)
					glVertexAttribPointer(
						/* index     = */ 1,
						/* size      = */ 4,
						/* type      = */ GL_FLOAT,
						/* normalize = */ GL_FALSE,
						/* stride    = */ sizeof(dd::DrawVertex),
						/* offset    = */ reinterpret_cast<void *>(offset));

					checkGLError(__FILE__, __LINE__);

					// VAOs can be a pain in the neck if left enabled...
					glBindVertexArray(0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}

				//
				// Text rendering vertex buffer:
				//
				{
					glGenVertexArrays(1, &m_textVAO);
					glGenBuffers(1, &m_textVBO);
					checkGLError(__FILE__, __LINE__);

					glBindVertexArray(m_textVAO);
					glBindBuffer(GL_ARRAY_BUFFER, m_textVBO);

					// NOTE: A more optimized implementation might consider combining
					// both the lines/points and text buffers to save some memory!
					glBufferData(GL_ARRAY_BUFFER, DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex), nullptr, GL_STREAM_DRAW);
					checkGLError(__FILE__, __LINE__);

					// Set the vertex format expected by the 2D text:
					std::size_t offset = 0;

					glEnableVertexAttribArray(0); // in_Position (vec2)
					glVertexAttribPointer(
						/* index     = */ 0,
						/* size      = */ 2,
						/* type      = */ GL_FLOAT,
						/* normalize = */ GL_FALSE,
						/* stride    = */ sizeof(dd::DrawVertex),
						/* offset    = */ reinterpret_cast<void *>(offset));
					offset += sizeof(float) * 2;

					glEnableVertexAttribArray(1); // in_TexCoords (vec2)
					glVertexAttribPointer(
						/* index     = */ 1,
						/* size      = */ 2,
						/* type      = */ GL_FLOAT,
						/* normalize = */ GL_FALSE,
						/* stride    = */ sizeof(dd::DrawVertex),
						/* offset    = */ reinterpret_cast<void *>(offset));
					offset += sizeof(float) * 2;

					glEnableVertexAttribArray(2); // in_Color (vec4)
					glVertexAttribPointer(
						/* index     = */ 2,
						/* size      = */ 4,
						/* type      = */ GL_FLOAT,
						/* normalize = */ GL_FALSE,
						/* stride    = */ sizeof(dd::DrawVertex),
						/* offset    = */ reinterpret_cast<void *>(offset));

					checkGLError(__FILE__, __LINE__);

					// Ditto.
					glBindVertexArray(0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}
			}

			static GLuint handleToGL(dd::GlyphTextureHandle handle)
			{
				const std::size_t temp = reinterpret_cast<std::size_t>(handle);
				return static_cast<GLuint>(temp);
			}

			static dd::GlyphTextureHandle GLToHandle(const GLuint id) 
			{
				const std::size_t temp = static_cast<std::size_t>(id);
				return reinterpret_cast<dd::GlyphTextureHandle>(temp);
			}

			static void checkGLError(const char * file, const int line)
			{
				GLenum err;
				while ((err = glGetError()) != GL_NO_ERROR)
				{
					BL_LOG_ERROR("%s(%d) : GL_CORE_ERROR=0x%X - %s", file, line, err);
				}
			}

			static void compileShader(const GLuint shader)
			{
				glCompileShader(shader);
				checkGLError(__FILE__, __LINE__);

				GLint status;
				glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
				checkGLError(__FILE__, __LINE__);

				if (status == GL_FALSE)
				{
					GLchar strInfoLog[1024] = { 0 };
					glGetShaderInfoLog(shader, sizeof(strInfoLog) - 1, nullptr, strInfoLog);
				}
			}

			static void linkProgram(const GLuint program)
			{
				glLinkProgram(program);
				checkGLError(__FILE__, __LINE__);

				GLint status;
				glGetProgramiv(program, GL_LINK_STATUS, &status);
				checkGLError(__FILE__, __LINE__);

				if (status == GL_FALSE)
				{
					GLchar strInfoLog[1024] = { 0 };
					glGetProgramInfoLog(program, sizeof(strInfoLog) - 1, nullptr, strInfoLog);
				}
			}

		private:

			glm::mat4 m_mvpMatrix;

			GLuint m_linePointProgram;
			GLint  m_linePointProgram_MvpMatrixLocation;

			GLuint m_textProgram;
			GLint  m_textProgram_GlyphTextureLocation;
			GLint  m_textProgram_ScreenDimensions;

			GLuint m_linePointVAO;
			GLuint m_linePointVBO;

			GLuint m_textVAO;
			GLuint m_textVBO;

			static const char * linePointVertShaderSrc;
			static const char * linePointFragShaderSrc;

			static const char * textVertShaderSrc;
			static const char * textFragShaderSrc;

			float m_windowHeight;
			float m_windowWidth;

		}; 
		
		const char * DebugDrawRender::linePointVertShaderSrc = "\n"
			"#version 150\n"
			"\n"
			"in vec3 in_Position;\n"
			"in vec4 in_ColorPointSize;\n"
			"\n"
			"out vec4 v_Color;\n"
			"uniform mat4 u_MvpMatrix;\n"
			"\n"
			"void main()\n"
			"{\n"
			"    gl_Position  = u_MvpMatrix * vec4(in_Position, 1.0);\n"
			"    gl_PointSize = 15.0;\n"
			"    v_Color      = vec4(in_ColorPointSize.xyz, 1.0);\n"
			"}\n";

		const char * DebugDrawRender::linePointFragShaderSrc = "\n"
			"#version 150\n" 
			"\n"
			"in  vec4 v_Color;\n"
			"out vec4 out_FragColor;\n"
			"\n"
			"void main()\n"
			"{\n"
			"    out_FragColor = v_Color;\n"
			"}\n";

		const char * DebugDrawRender::textVertShaderSrc = "\n"
			"#version 150\n"
			"\n"
			"in vec2 in_Position;\n"
			"in vec2 in_TexCoords;\n"
			"in vec3 in_Color;\n"
			"\n"
			"uniform vec2 u_screenDimensions;\n"
			"\n"
			"out vec2 v_TexCoords;\n"
			"out vec4 v_Color;\n"
			"\n"
			"void main()\n"
			"{\n"
			"    // Map to normalized clip coordinates:\n"
			"    float x = ((2.0 * (in_Position.x - 0.5)) / u_screenDimensions.x) - 1.0;\n"
			"    float y = 1.0 - ((2.0 * (in_Position.y - 0.5)) / u_screenDimensions.y);\n"
			"\n"
			"    gl_Position = vec4(x, y, 0.0, 1.0);\n"
			"    v_TexCoords = in_TexCoords;\n"
			"    v_Color     = vec4(in_Color, 1.0);\n"
			"}\n";

		const char * DebugDrawRender::textFragShaderSrc = "\n"
			"#version 150\n"
			"\n"
			"in vec2 v_TexCoords;\n"
			"in vec4 v_Color;\n"
			"\n"
			"uniform sampler2D u_glyphTexture;\n"
			"out vec4 out_FragColor;\n"
			"\n"
			"void main()\n"
			"{\n"
			"    out_FragColor = v_Color;\n"
			"    out_FragColor.a = texture(u_glyphTexture, v_TexCoords).r;\n"
			"}\n";
	}
}

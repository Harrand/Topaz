//
// Created by Harrand on 13/12/2019.
//

#include "core/core.hpp"
#include "glad_context.hpp"
#include "core/debug/assert.hpp"
#include "core/debug/print.hpp"

#include "GLFW/glfw3.h"

namespace tz::ext::glad
{
	namespace debug
	{
		void output(GLenum source, GLenum type, GLuint id, GLenum severity, 
                            GLsizei length, const GLchar *message, const void *userParam);
	}

	GLADContext::GLADContext(const tz::ext::glfw::GLFWContext& glfw_context) noexcept: glfw_context(&glfw_context), loaded(false){}
	
	GLADContext::GLADContext() noexcept: glfw_context(nullptr), loaded(false){}
	
	void GLADContext::load()
	{
		if(this->glfw_context == nullptr)
		{
			// If we never were given a glfw context, use the global one now.
			this->glfw_context = &tz::ext::glfw::get();
		}
		topaz_assert(!this->loaded, "GLADContext::load(): Context already marked as loaded!");
		topaz_assert(this->glfw_context->has_window(), "GLADContext::load(): Given GLFW context is incomplete -- It doesn't have a valid window attached to it.");
		// Load all the things!
		gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

		#if TOPAZ_DEBUG
			tz::debug_printf("Activating OpenGL debug context...\n");
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

			glEnable(GL_DEBUG_OUTPUT);
    		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
   			glDebugMessageCallback(debug::output, nullptr);
    		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		#endif
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
	}
	
	bool GLADContext::is_loaded() const
	{
		return this->loaded;
	}
	
	void load_opengl()
	{
		topaz_assert(!global_context.is_loaded(), "tz::ext::glad::load_opengl(): Global GLAD context is already loaded!");
		global_context = GLADContext{}; // Re-assign incase a new glfw context was since bound.
		global_context.load(); // Let's go!
	}
	
	void load_opengl(const tz::ext::glfw::GLFWContext& specific_context)
	{
		topaz_assert(!global_context.is_loaded(), "tz::ext::glad::load_opengl(): Global GLAD context is already loaded!");
		global_context = GLADContext{specific_context}; // Re-assign using the context we were told to use.
		global_context.load(); // Let's go!
	}

	void custom_error(GLenum severity, const GLchar *message)
	{
		debug::output(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, severity, -1, message, nullptr);
	}

	std::pair<int, int> gl_version()
	{
		return {GLVersion.major, GLVersion.minor};
	}

	namespace debug
	{
		void output(GLenum source, 
                            GLenum type, 
                            GLuint id, 
                            GLenum severity, 
                            GLsizei length, 
                            const GLchar *message, 
                            const void *userParam)
		{
			// ignore non-significant error/warning codes
			if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

			std::cout << "---------------" << std::endl;
			std::cout << "Debug message (" << id << "): " <<  message << std::endl;

			switch (source)
			{
				case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
				case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
				case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
				case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
				case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
				case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
			} std::cout << std::endl;

			switch (type)
			{
				case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
				case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
				case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
				case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
				case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
				case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
				case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
			} std::cout << std::endl;
			
			switch (severity)
			{
				case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
				case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
				case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
				case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
			} std::cout << std::endl;
			std::cout << std::endl;
		}
	}
}
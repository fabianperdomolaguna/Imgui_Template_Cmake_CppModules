module;

#include <string>

#include "glad/gl.h"

export module beryl.utils.gl;

import beryl.logger;

const char* GetSourceStr(GLenum source) {
    switch (source) {
        case GL_DEBUG_SOURCE_API:             return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "Window System";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
        case GL_DEBUG_SOURCE_THIRD_PARTY:     return "Third Party";
        case GL_DEBUG_SOURCE_APPLICATION:     return "Application";
        default:                              return "Other";
    }
}

const char* GetTypeStr(GLenum type) {
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               return "Error";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "Undefined";
        case GL_DEBUG_TYPE_PORTABILITY:         return "Portability";
        case GL_DEBUG_TYPE_PERFORMANCE:         return "Performance";
        default:                                return "Other";
    }
}

void OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar* message, const void* userParam)
{
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    if (severity == GL_DEBUG_SEVERITY_HIGH)
    {
        beryl::logger::Error("OpenGL Debug Message", 
            "id", id, 
            "source", GetSourceStr(source), 
            "type", GetTypeStr(type), 
            "details", message);
    }
    else if (severity == GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_LOW)
    {
        beryl::logger::Warn("OpenGL Debug Message", 
            "id", id, 
            "source", GetSourceStr(source), 
            "type", GetTypeStr(type), 
            "details", message);
    }
    else
    {
        beryl::logger::Info("OpenGL Debug Message", 
            "id", id, 
            "details", message);
    }
}

export void EnableOpenGLDebug() 
{
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLDebugCallback, nullptr);
    
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    beryl::logger::Info("OpenGL Debug Output System Initialized");
}

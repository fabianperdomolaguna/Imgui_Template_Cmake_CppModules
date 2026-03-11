#pragma once

#include "glad/gl.h"

const char* GetSourceStr(GLenum source);
const char* GetTypeStr(GLenum type);

void OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar* message, const void* userParam);

namespace beryl::utils::gl
{
    void EnableOpenGLDebug();
}
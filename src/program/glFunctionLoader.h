#ifndef __GL_FUNCTION_LOADER_H__
#define __GL_FUNCTION_LOADER_H__
#include <GL/gl.h>
#include <string.h>
#include "windowsStuff.h"
extern void (*glDeleteProgram)(GLuint program);
extern void (*glDeleteBuffers)(GLsizei n,
                               const GLuint *buffers);
extern void (*glGenBuffers)(GLsizei n, GLuint *buffers);
extern GLuint (*glCreateShader)(GLenum shaderType);
extern void (*glShaderSource)(GLuint shader,
                              GLsizei count,
                              const GLchar **str,
                              const GLint *length);
extern void (*glCompileShader)(GLuint shader);
extern void (*glGetShaderiv)(GLuint shader,
                             GLenum pname,
                             GLint *params);
extern void (*glGetShaderInfoLog)(GLuint shader,
                                  GLsizei maxLength,
                                  GLsizei *length,
                                  GLchar *infoLog);
extern void (*glGetProgramInfoLog)(GLuint program,
                                   GLsizei maxLength,
                                   GLsizei *length,
                                   GLchar *infoLog);
extern void (*glGetProgramiv)(GLuint program,
                              GLenum pname,
                              GLint *params);
extern GLuint (*glCreateProgram)(void);
extern void (*glAttachShader)(GLuint program,
                              GLuint shader);
extern void (*glLinkProgram)(GLuint program);
extern void (*glDeleteShader)(GLuint shader);
extern void (*glGenVertexArrays)(GLsizei n,
                                 GLuint *arrays);
extern void (*GenBuffers)(GLsizei n,
                          GLuint *buffers);
extern void (*glBindVertexArray)(GLuint array);
extern void (*glBindBuffer)(GLenum target,
                            GLuint buffer);
extern void (*glBufferData)(GLenum target,
                            GLsizeiptr size,
                            const void *data,
                            GLenum usage);
extern void (*glVertexAttribPointer)(GLuint index,
                                     GLint size,
                                     GLenum type,
                                     GLboolean normalized,
                                     GLsizei stride,
                                     const void *pointer);
extern void (*glEnableVertexAttribArray)(GLuint index);
extern void (*glUseProgram)(GLuint program);
extern void (*glDeleteVertexArrays)(GLsizei n,
                                    const GLuint *arrays);
extern void (*DeleteBuffers)(GLsizei n,
                             const GLuint *buffers);
extern void (*DeleteProgram)(GLuint program);
extern GLint (*glGetUniformLocation)(GLuint program, const GLchar *uniformName);
extern void (*glUniform1f)(GLuint id, GLfloat x);
extern void (*glUniform2f)(GLuint id, GLfloat x, GLfloat y);
extern void (*glUniform3f)(GLuint id, GLfloat x, GLfloat y, GLfloat z);

extern void (*glUniform2i)(GLuint id, GLuint x, GLuint y);
extern void (*glUniform1i)(GLuint id, GLint x);
#define bindFunctionRef(a, b)                 \
    {                                         \
        void *p = a;                          \
        void *fp = (b)glfwGetProcAddress(#a); \
        p = fp;                               \
    }
#define bindFunction(a, b) a = (b)glfwGetProcAddress(#a);

void loadOpenGLFunctions(void);
#ifdef GL_FUNCTION_LOADER__IMPLEMENTATION
void (*glDeleteProgram)(GLuint program);
void (*glDeleteBuffers)(GLsizei n,
                        const GLuint *buffers);
void (*glGenBuffers)(GLsizei n, GLuint *buffers);
GLuint (*glCreateShader)(GLenum shaderType);
void (*glShaderSource)(GLuint shader,
                       GLsizei count,
                       const GLchar **str,
                       const GLint *length);
void (*glCompileShader)(GLuint shader);
void (*glGetShaderiv)(GLuint shader,
                      GLenum pname,
                      GLint *params);
void (*glGetShaderInfoLog)(GLuint shader,
                           GLsizei maxLength,
                           GLsizei *length,
                           GLchar *infoLog);
void (*glGetProgramInfoLog)(GLuint program,
                            GLsizei maxLength,
                            GLsizei *length,
                            GLchar *infoLog);
void (*glGetProgramiv)(GLuint program,
                       GLenum pname,
                       GLint *params);
GLuint (*glCreateProgram)(void);
void (*glAttachShader)(GLuint program,
                       GLuint shader);
void (*glLinkProgram)(GLuint program);
void (*glDeleteShader)(GLuint shader);
void (*glGenVertexArrays)(GLsizei n,
                          GLuint *arrays);
void (*GenBuffers)(GLsizei n,
                   GLuint *buffers);
void (*glBindVertexArray)(GLuint array);
void (*glBindBuffer)(GLenum target,
                     GLuint buffer);
void (*glBufferData)(GLenum target,
                     GLsizeiptr size,
                     const void *data,
                     GLenum usage);
void (*glVertexAttribPointer)(GLuint index,
                              GLint size,
                              GLenum type,
                              GLboolean normalized,
                              GLsizei stride,
                              const void *pointer);
void (*glEnableVertexAttribArray)(GLuint index);
void (*glUseProgram)(GLuint program);
void (*glDeleteVertexArrays)(GLsizei n,
                             const GLuint *arrays);
void (*DeleteBuffers)(GLsizei n,
                      const GLuint *buffers);
void (*DeleteProgram)(GLuint program);
GLint (*glGetUniformLocation)(GLuint program, const GLchar *uniformName);
void (*glUniform1f)(GLuint id, GLfloat x);
void (*glUniform2f)(GLuint id, GLfloat x, GLfloat y);
void (*glUniform3f)(GLuint id, GLfloat x, GLfloat y, GLfloat z);
// void (*glUniform4f)(GLuint id, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void (*glUniform2i)(GLuint id, GLuint x, GLuint y);
void (*glUniform1i)(GLuint id, GLint x);
void loadOpenGLFunctions(void)
{
    bindFunctionRef(glViewport, void (*)(GLint x, GLint y, GLsizei width, GLsizei height));
    bindFunction(glUniform1i, void (*)(GLuint id, GLint x));
    bindFunction(glUniform1f, void (*)(GLuint id, GLfloat x));
    bindFunction(glUniform2f, void (*)(GLuint id, GLfloat x, GLfloat y));
    bindFunction(glUniform3f, void (*)(GLuint id, GLfloat x, GLfloat y, GLfloat z));
    bindFunction(glUniform4f, void (*)(GLuint id, GLfloat x, GLfloat y, GLfloat z, GLfloat w));
    bindFunction(glUniform2i, void (*)(GLuint id, GLuint x, GLuint y));
    bindFunction(glGetUniformLocation, GLint(*)(GLuint program, const GLchar *uniformName));
    bindFunctionRef(glClear, void (*)(GLbitfield));
    bindFunctionRef(glClearColor, void (*)(GLclampf r, GLclampf g, GLclampf b, GLclampf a));
    bindFunction(glCreateShader, GLuint(*)(GLenum shaderType));
    bindFunction(glShaderSource, void (*)(GLuint shader, GLsizei count, const GLchar **string, const GLint *length));
    bindFunction(glCompileShader, void (*)(GLuint shader));
    bindFunction(glGetShaderiv, void (*)(GLuint shader,
                                         GLenum pname,
                                         GLint * params));
    bindFunction(glGetShaderInfoLog, void (*)(GLuint shader,
                                              GLsizei maxLength,
                                              GLsizei * length,
                                              GLchar * infoLog));
    bindFunction(glGetProgramInfoLog, void (*)(GLuint program,
                                               GLsizei maxLength,
                                               GLsizei * length,
                                               GLchar * infoLog));
    bindFunction(glGetProgramiv, void (*)(GLuint program,
                                          GLenum pname,
                                          GLint * params));
    bindFunction(glCreateProgram, GLuint(*)(void));
    bindFunction(glAttachShader, void (*)(GLuint program,
                                          GLuint shader));
    bindFunction(glLinkProgram, void (*)(GLuint program));
    bindFunction(glDeleteShader, void (*)(GLuint shader));
    bindFunction(glGenVertexArrays, void (*)(GLsizei n, GLuint * arrays));
    bindFunction(glGenBuffers, void (*)(GLsizei n, GLuint * buffers));
    bindFunction(glBindVertexArray, void (*)(GLuint array));
    bindFunction(glBindBuffer, void (*)(GLenum target, GLuint buffer));
    bindFunction(glBufferData, void (*)(GLenum target,
                                        GLsizeiptr size,
                                        const void *data,
                                        GLenum usage));
    bindFunction(glVertexAttribPointer, void (*)(GLuint index,
                                                 GLint size,
                                                 GLenum type,
                                                 GLboolean normalized,
                                                 GLsizei stride,
                                                 const void *pointer));
    bindFunction(glEnableVertexAttribArray, void (*)(GLuint index));
    bindFunction(glUseProgram, void (*)(GLuint program));
    bindFunction(glDeleteVertexArrays, void (*)(GLsizei n, const GLuint *arrays));
    bindFunction(glDeleteBuffers, void (*)(GLsizei n, const GLuint *buffers));
    bindFunction(glDeleteProgram, void (*)(GLuint program));
#if defined(_WIN32) || defined(_WIN64)
    bindFunction(glActiveTexture, void (*)(GLenum texture));
#endif
}
#undef GL_FUNCTION_LOADER__IMPLEMENTATION
#endif
#endif
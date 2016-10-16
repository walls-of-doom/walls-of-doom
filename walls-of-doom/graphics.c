#include "graphics.h"

#include "constants.h"
#include "data.h"
#include "logger.h"
#include "memory.h"
#include "text.h"

#include <GL/glew.h>

#define SHADER_BUFFER_SIZE 8192

static void graphics_log_information(const GLuint shader_handle) {
  GLint length = 0;
  char *shader_log = NULL;
  /* Get the log length. */
  glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &length);
  if (length == 0) {
    log_message("Shader has no information log!");
    return;
  }
  /* Dynamically allocate enough memory for the shader log. */
  shader_log = resize_memory(shader_log, length);
  glGetShaderInfoLog(shader_handle, length, NULL, shader_log);
  trim_string(shader_log);
  log_message(shader_log);
  resize_memory(shader_log, 0);
}

static char *get_vertex_shader_source(void) {
  char *source = NULL;
  source = resize_memory(source, SHADER_BUFFER_SIZE);
  read_characters(VERTEX_SHADER_PATH, source, SHADER_BUFFER_SIZE);
  return source;
}

static char *get_fragment_shader_source(void) {
  char *source = NULL;
  source = resize_memory(source, SHADER_BUFFER_SIZE);
  read_characters(FRAGMENT_SHADER_PATH, source, SHADER_BUFFER_SIZE);
  return source;
}

static void initialize_vertex_shader(void) {
  GLint compilation_status;
  GLuint shader_handle = glCreateShader(GL_VERTEX_SHADER);
  const char *shader_source = get_vertex_shader_source();
  glShaderSource(shader_handle, 1, &shader_source, NULL);
  shader_source = resize_memory((void *)shader_source, 0);
  glCompileShader(shader_handle);
  glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &compilation_status);
  if (compilation_status != GL_TRUE) {
    log_message("Failed to compile the vertex shader!");
  }
  graphics_log_information(shader_handle);
}

static void initialize_fragment_shader(void) {
  GLint compilation_status;
  GLuint shader_handle = glCreateShader(GL_FRAGMENT_SHADER);
  const char *shader_source = get_fragment_shader_source();
  glShaderSource(shader_handle, 1, &shader_source, NULL);
  shader_source = resize_memory((void *)shader_source, 0);
  glCompileShader(shader_handle);
  glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &compilation_status);
  if (compilation_status != GL_TRUE) {
    log_message("Failed to compile the fragment shader!");
  }
  graphics_log_information(shader_handle);
}

void initialize_shaders(void) {
  initialize_vertex_shader();
  initialize_fragment_shader();
}

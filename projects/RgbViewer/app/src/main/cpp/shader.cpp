#define LOG_TAG "Shader"

#include "logging.h"
#include "shader.h"

Shader::Shader(const char *vertex_code, const char *fragment_code)
    : linked_(false),
      vertex_code_(vertex_code),
      fragment_code_(fragment_code) {}

void Shader::Link() {
    LOGI("called");
    // 1. compile shaders
    GLuint vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    const char *vertex_code = vertex_code_.c_str();
    glShaderSource(vertex, 1, &vertex_code, nullptr);
    glCompileShader(vertex);
    CheckCompileErrors(vertex, "VERTEX");

    // fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fragment_code = fragment_code_.c_str();
    glShaderSource(fragment, 1, &fragment_code, nullptr);
    glCompileShader(fragment);
    CheckCompileErrors(fragment, "FRAGMENT");
    // shader program
    id_ = glCreateProgram();
    glAttachShader(id_, vertex);
    glAttachShader(id_, fragment);
    for (auto &attribute : attributes_) {
        GLint index = attribute.first;
        const char *name = attribute.second.c_str();
        glBindAttribLocation(id_, index, name);
        if (CheckGlError("glBindAttribLocation")) {
            LOGE("bind %d to %s failed", index, name);
        }
    }
    glLinkProgram(id_);
    CheckCompileErrors(id_, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessary.
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    linked_ = true;
}

void Shader::BindAttribLocation(int index, const std::string &name) {
    attributes_[index] = name;
}

void Shader::Use() {
    if (!linked_) Link();
    glUseProgram(id_);
}

void Shader::SetBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(id_, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
}

void Shader::SetFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
}

void Shader::CheckCompileErrors(unsigned int shader, const std::string &type) {
    int success;
    char info_log[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, info_log);
            LOGE("ERROR::SHADER_COMPILATION_ERROR of type: %s, info: %s", type.c_str(), info_log);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, info_log);
            LOGE("ERROR::PROGRAM_LINKING_ERROR of type: %s, info: %s", type.c_str(), info_log);
        }
    }
}

bool Shader::CheckGlError(const char *op) {
    GLint error;

    for (error = glGetError(); error; error = glGetError()) {
        LOGE("after %s() glError (0x%x)\n", op, error);
        return true;
    }

    return false;
}

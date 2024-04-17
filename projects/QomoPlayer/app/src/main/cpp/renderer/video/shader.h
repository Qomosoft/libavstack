#pragma once

#include <string>
#include <unordered_map>
#include <GLES3/gl3.h>

class Shader {
 public:
  Shader(const char *vertex_code, const char *fragment_code);

  void Link();

  void Use();

  void SetBool(const std::string &name, bool value) const;

  void SetInt(const std::string &name, int value) const;

  void SetFloat(const std::string &name, float value) const;

 private:
  void CheckCompileErrors(unsigned int shader, const std::string &type);

  bool CheckGlError(const char* op);


 private:
  GLuint id_;
  bool linked_;
  std::string vertex_code_;
  std::string fragment_code_;
};
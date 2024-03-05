#pragma once

#include <string>
#include <unordered_map>
#include <GLES2/gl2.h>

class Shader {
 public:
  Shader(const char *vertex_code, const char *fragment_code);

  void Link();

  void BindAttribLocation(int index, const std::string &name);

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
  std::unordered_map<int, std::string> attributes_;
};
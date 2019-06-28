#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <glad/glad.h>
#include "core/types.hpp"
#include <unordered_map>
#include <vector>

// ========================================================================== //
// Shader Declaration
// ========================================================================== //

namespace dib::graphics {

/** Shader program **/
class ShaderProgram
{
  friend class ShaderManager;

private:
  /** Program ID**/
  GLuint mProgramID;

  /** Program name **/
  String mName;
  /** Path to vertex shader **/
  String mVsPath;
  /** Path to fragment shader **/
  String mFsPath;

public:
  /** Bind shader program **/
  void Bind();

  /** Returns the location of a uniform **/
  GLint GetUniformLocation(const String& name);

  /** Set uniform s32 **/
  void SetUniformS32(const String& name, s32 value);

  /** Set uniform s32 **/
  void SetUniformF32(const String& name, f32 value);

  /** Set uniform matrix4 **/
  void SetUniformMatrix4(const String& name,
                         const glm::mat4& matrix,
                         bool transpose = false);

  /** Set uniform vec3 **/
  void SetUniformVector3(const String& name, const glm::vec3& vector);

  /** Set uniform vec4 **/
  void SetUniformVector4(const String& name, const glm::vec4& vector);

  /** Reload shader **/
  bool Reload();

  /** Returns the OpenGL program ID **/
  GLuint GetID() const { return mProgramID; }

private:
  /** Construct a shader program with a path to a vertex shader and fragment
   * shader. A name can be set for the program, it defaults to empty **/
  ShaderProgram(const String& vsPath,
                const String& fsPath,
                const String& programName = "");

  /** Destruct shader program **/
  ~ShaderProgram();

  /** Load shader program from source **/
  GLuint LoadProgram();

  String LoadShaderSource(const String& path);

  GLuint CompileShader(const String& path, const String& source, GLenum type);

  void OnShaderCompileError(const String& path, GLuint shaderID);

  void OnProgramError();
};

}

// ========================================================================== //
// ShaderManager Declaration
// ========================================================================== //

namespace dib::graphics {

/** Shader manager **/
class ShaderManager
{
private:
  /** Map of all loaded shaders **/
  std::unordered_map<String, ShaderProgram*> mShaderPrograms;

private:
  ShaderManager() = default;

  ~ShaderManager();

public:
  /** Returns the shader manager instance **/
  static ShaderManager& Instance();

  /** Load and return a shader program **/
  static ShaderProgram* LoadProgram(const String& name,
                                    const String& vsPath,
                                    const String& fsPath);

  /** Returns a shader program by name **/
  static ShaderProgram* GetProgram(const String& name);

  /** Reload shaders **/
  static void Reload();
};

}

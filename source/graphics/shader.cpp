#include "graphics/shader.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <dlog.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <alflib/file/file_io.hpp>

// ========================================================================== //
// Shader Implementation
// ========================================================================== //

namespace dib::graphics {

void
ShaderProgram::Bind()
{
  glUseProgram(mProgramID);
}

// -------------------------------------------------------------------------- //

GLint
ShaderProgram::GetUniformLocation(const String& name)
{
  return glGetUniformLocation(mProgramID, name.GetUTF8());
}

// -------------------------------------------------------------------------- //

void
ShaderProgram::SetUniformS32(const String& name, s32 value)
{
  GLint location = GetUniformLocation(name);
  glUniform1i(location, value);
}

// -------------------------------------------------------------------------- //

void
ShaderProgram::SetUniformF32(const String& name, f32 value)
{
  GLint location = GetUniformLocation(name);
  glUniform1f(location, value);
}

// -------------------------------------------------------------------------- //

void
ShaderProgram::SetUniformMatrix4(const String& name,
                                 const glm::mat4& matrix,
                                 bool transpose)
{
  GLint location = GetUniformLocation(name);
  glUniformMatrix4fv(location, 1, transpose, glm::value_ptr(matrix));
}

// -------------------------------------------------------------------------- //

void
ShaderProgram::SetUniformVector3(const String& name, const glm::vec3& vector)
{
  GLint location = GetUniformLocation(name);
  glUniform3f(location, vector.x, vector.y, vector.z);
}

// -------------------------------------------------------------------------- //

void
ShaderProgram::SetUniformVector4(const String& name, const glm::vec4& vector)
{
  GLint location = GetUniformLocation(name);
  glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

// -------------------------------------------------------------------------- //

bool
ShaderProgram::Reload()
{
  GLuint program = LoadProgram();
  if (!program) {
    DLOG_ERROR("Failed to reload shader, old program will remain");
    return false;
  }
  mProgramID = program;
  DLOG_VERBOSE("Successfully reloaded ShaderProgram{{name = \"{}\"}}", mName);
  return true;
}

// -------------------------------------------------------------------------- //

ShaderProgram::ShaderProgram(const String& vsPath,
                             const String& fsPath,
                             const String& programName)
  : mName(programName)
  , mVsPath(vsPath)
  , mFsPath(fsPath)
{
  // Compile shader program
  mProgramID = LoadProgram();
  if (!mProgramID) {
    DLOG_ERROR("Failed to compile shader program");
    std::exit(-1);
  }

  // Print status
  DLOG_VERBOSE(
    "Successfully compiled ShaderProgram{{vs = \"{}\", fs = \"{}\"}}",
    vsPath,
    fsPath);
}

// -------------------------------------------------------------------------- //

ShaderProgram::~ShaderProgram()
{
  glDeleteProgram(mProgramID);
  mProgramID = 0;
}

// -------------------------------------------------------------------------- //

GLuint
ShaderProgram::LoadProgram()
{
  // Load shader sources
  const String vsSource = LoadShaderSource(mVsPath);
  const String fsSource = LoadShaderSource(mFsPath);
  if (vsSource.GetLength() < 1 || fsSource.GetLength() < 1) {
    return 0;
  }

  // Compile shaders
  const GLuint vs = CompileShader(mVsPath, vsSource, GL_VERTEX_SHADER);
  if (!vs) {
    return 0;
  }
  const GLuint fs = CompileShader(mFsPath, fsSource, GL_FRAGMENT_SHADER);
  if (!fs) {
    glDeleteShader(vs);
    return 0;
  }

  // Create program
  const GLuint programID = glCreateProgram();

  // Attach shaders to the program
  glAttachShader(programID, vs);
  glAttachShader(programID, fs);

  // Link program
  glLinkProgram(programID);

  // Check linking status
  GLint linked;
  glGetProgramiv(programID, GL_LINK_STATUS, &linked);
  if (!linked) {
    OnProgramError();
    glDeleteProgram(programID);
    glDeleteShader(vs);
    glDeleteShader(fs);
    DLOG_ERROR("Failed to link shader program");
    return 0;
  }

  // Clean up
  glDetachShader(programID, vs);
  glDetachShader(programID, fs);
  // glDeleteShader(vs);
  // glDeleteShader(fs);

  // Set program name
  if (mName.GetLength() > 0) {
    glObjectLabel(GL_PROGRAM,
                  programID,
                  static_cast<GLsizei>(mName.GetSize()),
                  mName.GetUTF8());
  }

  return programID;
}

// -------------------------------------------------------------------------- //

String
ShaderProgram::LoadShaderSource(const String& path)
{
  // Read source
  alflib::FileIO io(Path{ path });
  alflib::FileResult result = io.Open(alflib::FileIO::Flag::kRead);
  if (result != alflib::FileResult::kSuccess) {
    return "";
  }
  String output;
  if (io.Read(output) != alflib::FileResult::kSuccess) {
    return "";
  }
  return output;
}

// -------------------------------------------------------------------------- //

GLuint
ShaderProgram::CompileShader(const String& path,
                             const String& source,
                             GLenum type)
{
  // Compile fragment shader
  GLuint id = glCreateShader(type);
  const char8* _source = source.GetUTF8();
  glShaderSource(id, 1, &_source, nullptr);
  glCompileShader(id);
  GLint status;
  glGetShaderiv(id, GL_COMPILE_STATUS, &status);
  if (!status) {
    OnShaderCompileError(path, id);
    glDeleteShader(id);
    return 0;
  }
  return id;
}

// -------------------------------------------------------------------------- //

void
ShaderProgram::OnShaderCompileError(const String& path, GLuint shaderID)
{
  GLint infoLogLength = 0;
  glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
  std::vector<char8> infoLog(infoLogLength);
  glGetShaderInfoLog(shaderID, infoLogLength, &infoLogLength, &infoLog[0]);
  std::string infoLogStr(infoLog.begin(), infoLog.end());
  DLOG_ERROR("Failed to compile shader \"{}\":\n{}", path, infoLogStr);
}

// -------------------------------------------------------------------------- //

void
ShaderProgram::OnProgramError()
{
  fmt::print("Failed to link shader program\n");
  GLint infoLogLength = 0;
  glGetProgramiv(mProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
  std::vector<char> infoLog(infoLogLength);
  glGetProgramInfoLog(mProgramID, infoLogLength, &infoLogLength, &infoLog[0]);
  std::string infoLogStr(infoLog.begin(), infoLog.end());
  DLOG_ERROR("{}\n", infoLogStr);
}

}

// ========================================================================== //
// ShaderManager Implementation
// ========================================================================== //

namespace dib::graphics {

ShaderManager::~ShaderManager() {}

// -------------------------------------------------------------------------- //

ShaderManager&
ShaderManager::Instance()
{
  static ShaderManager manager;
  return manager;
}

// -------------------------------------------------------------------------- //

ShaderProgram*
ShaderManager::LoadProgram(const String& name,
                           const String& vsPath,
                           const String& fsPath)
{
  ShaderManager& manager = Instance();
  if (manager.mShaderPrograms.count(name) < 1) {
    ShaderProgram* program = new ShaderProgram(vsPath, fsPath, name);
    manager.mShaderPrograms[name] = program;
  }
  return manager.mShaderPrograms[name];
}

// -------------------------------------------------------------------------- //

ShaderProgram*
ShaderManager::GetProgram(const String& name)
{
  ShaderManager& manager = Instance();
  if (manager.mShaderPrograms.count(name) < 1) {
    return nullptr;
  }
  return manager.mShaderPrograms[name];
}

// -------------------------------------------------------------------------- //

void
ShaderManager::Reload()
{
  ShaderManager& manager = Instance();
  for (auto& it : manager.mShaderPrograms) {
    ShaderProgram* program = it.second;
    program->Reload();
  }
}

// -------------------------------------------------------------------------- //

void
ShaderManager::UnloadAll()
{
  ShaderManager& manager = Instance();
  for (auto& it : manager.mShaderPrograms) {
    ShaderProgram* program = it.second;
    delete program;
  }
}

}
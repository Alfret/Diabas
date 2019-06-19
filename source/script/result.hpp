#pragma once

// ========================================================================== //
// Result Enumeration
// ========================================================================== //

namespace dib::script {

/** Script results **/
enum class Result
{
  /** Success **/
  kSuccess,
  /** Unknown error **/
  kUnknownError,
  /** Could not find module **/
  kModuleNotFound,
  /** Could not read module file **/
  kFailedToReadFile,
  /** Compilation error **/
  kCompileError,
  /** Exception occurred **/
  kException,
  /** Class was not found **/
  kClassNotFound
};

}
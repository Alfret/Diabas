#ifndef INPUT_HPP_
#define INPUT_HPP_

// ========================================================================== //
// Headers
// ========================================================================== //

#include <functional>
#include <future>
#include <game/world.hpp>
#include <string_view>

#include "core/types.hpp"

namespace dib {

// ========================================================================== //
// InputCommandCategory Enumeration
// ========================================================================== //

enum class InputCommandCategory
{
  kUnknown,
  kInfo,
  kSystem,
  kChat
};

// ========================================================================== //
// AsyncGetline Declaration
// ========================================================================== //

/** Class to asynchronously get input from the command-line. Adapter from:
 * https://stackoverflow.com/questions/16592357/non-blocking-stdgetline-exit-if-no-input
 */
class AsyncGetline
{
private:
  /** Whether to continue getting input **/
  std::atomic<bool> mContinueGettingInput;
  /** Send next line **/
  std::atomic<bool> mSendOverNextLine;
  /** Locking mutex **/
  std::mutex mInputLock;
  /** Input **/
  std::string mInput;

public:
  /** Construct and start getting lines **/
  AsyncGetline();

  /** Stop getting lines **/
  ~AsyncGetline();

  /** Get the input line **/
  std::string GetLine();
};

// ========================================================================== //
// CLIInputHandle Declaration
// ========================================================================== //

class InputCommand;

class CLIInputHandler
{
public:
  CLIInputHandler();

  ~CLIInputHandler();

  /**
   * Example, callback on a class instance:
   *
   * input_handler.AddCommand(InputCommandCategory::kInfo,
   *                          "command_string",
   *                          std::bind(&Class::Method, &class_instance,
   *                                    std::placeholders::_1));
   */
  void AddCommand(const InputCommandCategory category,
                  const alflib::String& command,
                  std::function<void(const std::string_view)> callback);

  void Update();

private:
  void RunCommand(const alflib::String& input) const;

private:
  std::vector<InputCommand> commands_;
  AsyncGetline async_getline_;
};
}

#endif // INPUT_HPP_

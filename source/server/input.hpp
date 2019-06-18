#ifndef INPUT_HPP_
#define INPUT_HPP_

#include <alflib/string.hpp>
#include <functional>
#include <future>
#include <game/world.hpp>
#include <string_view>

namespace dib {

alflib::String
GetLine();

// ============================================================ //

enum class InputCommandCategory
{
  kUnknown,
  kInfo,
  kChat
};

// ============================================================ //

struct InputCommand;

// ============================================================ //

template<Side side>
class InputHandler
{
public:
  InputHandler();

  ~InputHandler();

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
  std::future<alflib::String> input_{};
};
}

#endif // INPUT_HPP_

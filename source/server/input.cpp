#include "input.hpp"
#include <dlog.hpp>
#include <iostream>
#include <string>

namespace dib {

alflib::String
GetLine()
{
  std::string input{};
  std::getline(std::cin, input, '\n');
  return alflib::String{ input };
}

// ============================================================ //

class InputCommand
{
public:
  InputCommand(const InputCommandCategory category,
               const alflib::String& command,
               std::function<void(const std::string_view)> callback);

  static alflib::String CategoryToString(const InputCommandCategory category);

  static InputCommandCategory StringToCategory(const alflib::String& string);

  alflib::String GetCategoryAsString() const
  {
    return CategoryToString(category_);
  }

  InputCommandCategory GetCategory() const { return category_; }

  const alflib::String& GetCommand() const { return command_; }

  void Run(const alflib::String& input) const
  {
    callback_(std::string_view{ input.GetUTF8(), input.GetSize() });
  }

private:
  InputCommandCategory category_;
  alflib::String command_;
  std::function<void(const std::string_view)> callback_;
};

// ============================================================ //

InputCommand::InputCommand(const InputCommandCategory category,
                           const alflib::String& command,
                           std::function<void(const std::string_view)> callback)
  : category_(category)
  , command_(command)
  , callback_(callback)
{}

alflib::String
InputCommand::CategoryToString(const InputCommandCategory category)
{
  switch (category) {
    case InputCommandCategory::kInfo:
      return "info";

    case InputCommandCategory::kChat:
      return "chat";

    case InputCommandCategory::kUnknown:
    default:
      return "unknown";
  }
}

InputCommandCategory
InputCommand::StringToCategory(const alflib::String& string)
{
  InputCommandCategory category;
  if (string == "info") {
    category = InputCommandCategory::kInfo;
  } else if (string == "chat") {
    category = InputCommandCategory::kChat;
  } else {
    category = InputCommandCategory::kUnknown;
  }
  return category;
}

// ============================================================ //

template<>
InputHandler<Side::kServer>::InputHandler()
  : commands_()
{
  input_ = std::async(std::launch::async, GetLine);
}

template<>
InputHandler<Side::kClient>::InputHandler()
  : commands_()
{}

template<>
InputHandler<Side::kServer>::~InputHandler()
{}

template<>
InputHandler<Side::kClient>::~InputHandler()
{}

template<>
void
InputHandler<Side::kServer>::AddCommand(
  const InputCommandCategory category,
  const alflib::String& command,
  std::function<void(const std::string_view)> callback)
{
  commands_.emplace_back(category, command, callback);
}

template<>
void
InputHandler<Side::kClient>::AddCommand(
  [[maybe_unused]] const InputCommandCategory category,
  [[maybe_unused]] const alflib::String& command,
  [[maybe_unused]] std::function<void(const std::string_view)> callback)
{}

template<>
void
InputHandler<Side::kServer>::RunCommand(const alflib::String& input) const
{
  if (input.GetSize() < 1) {
    return;
  }

  InputCommandCategory category = InputCommandCategory::kUnknown;
  alflib::String command{};
  alflib::String command_input{};

  const s64 space = input.Find(" ");
  constexpr s64 kNotFound = -1;
  if (space == kNotFound) {
    // only a category
    category = InputCommand::StringToCategory(input);
  } else {
    // category and command
    category = InputCommand::StringToCategory(input.Substring(0, space));
    command = input.Substring(space + 1);

    if (const s64 space2 = command.Find(" "); space2 != kNotFound) {
      // category, command and command input
      command_input = command.Substring(space2 + 1);
      command = command.Substring(0, space2);
    }
  }

  if (category == InputCommandCategory::kUnknown) {
    DLOG_RAW("Unknown category, available categories are:\n"
             "\tinfo - Information type commands.\n"
             "\tchat - Send chat messages.\n");
    return;
  }

  bool found = false;
  for (const auto& input_command : commands_) {
    if (input_command.GetCategory() == category &&
        input_command.GetCommand() == command) {
      found = true;
      input_command.Run(command_input);
    }
  }

  if (!found) {
    std::string str{};
    for (const auto& input_command : commands_) {
      if (input_command.GetCategory() == category) {
        str += "\t" + input_command.GetCommand().GetStdString() + "\n";
      }
    }
    DLOG_RAW("Unknown command, available commands for [{}] are:\n{}",
             InputCommand::CategoryToString(category),
             str);
  }
}

template<>
void
InputHandler<Side::kClient>::RunCommand([
  [maybe_unused]] const alflib::String& command) const
{}

template<>
void
InputHandler<Side::kServer>::Update()
{
  const auto status = input_.wait_for(std::chrono::nanoseconds(0));
  if (status == std::future_status::ready) {
    RunCommand(input_.get());
    input_ = std::async(std::launch::async, GetLine);
  }
}

template<>
void
InputHandler<Side::kClient>::Update()
{}

}

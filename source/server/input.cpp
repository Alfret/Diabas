#include "input.hpp"
#include <iostream>
#include <string>
#include <dlog.hpp>

namespace dib
{

alflib::String
GetLine()
{
  std::string input{};
  std::getline(std::cin, input, '\n');
  return alflib::String{input};
}

// ============================================================ //

InputCommand::InputCommand(const Category category,
               const alflib::String& command,
                           std::function<void(alflib::String)> callback)
    : category_(category),
      command_(command),
      callback_(callback)
{

}

alflib::String
InputCommand::CategoryToString(const Category category)
{
  switch (category) {
    case InputCommand::Category::kInfo:
      return "info";

    case InputCommand::Category::kChat:
      return "chat";

    case InputCommand::Category::kNone:
      return "none";

    case InputCommand::Category::kUnknown:
    default:
      return "unknown";
  }
}

InputCommand::Category
InputCommand::StringToCategory(const alflib::String& string)
{
  Category category;
  if (string == "info") {
    category = Category::kInfo;
  } else if (string == "chat") {
    category = Category::kChat;
  }
  else {
    category = Category::kUnknown;
  }
  return category;
}

// ============================================================ //

template <>
InputHandler<Side::kServer>::InputHandler()
{
  input_ = std::async(std::launch::async, GetLine);
}

template<>
InputHandler<Side::kClient>::InputHandler() {}

template<>
void
InputHandler<Side::kServer>::AddCommand(const InputCommand::Category category,
                         const alflib::String& command,
                                        std::function<void(alflib::String)> callback)
{
  commands_.emplace_back(category, command, callback);
}

template<>
void
InputHandler<Side::kClient>::AddCommand(
  [[maybe_unused]] const InputCommand::Category category,
  [[maybe_unused]] const alflib::String& command,
  [[maybe_unused]] std::function<void(alflib::String)> callback) {}

template<>
void
InputHandler<Side::kServer>::RunCommand(const alflib::String& input)
{
  if (input.GetSize() < 1) {
    return;
  }

  InputCommand::Category category = InputCommand::Category::kUnknown;
  alflib::String command{};
  alflib::String command_input{};
  const s64 space = input.Find(" ");
  constexpr s64 kNotFound = -1;
  if (space == kNotFound) {
    // only a command
    category = InputCommand::Category::kNone;
    command = input;
  } else {
    // category and command
    category = InputCommand::StringToCategory(input.Substring(0, space));
    command = input.Substring(space + 1);

    if (const s64 space2 = command.Find(" "); space2 != kNotFound) {
      // category, command and command input
      command_input = command.Substring(space2+1);
      command = command.Substring(0, space2);
    }
  }

  if (category == InputCommand::Category::kUnknown) {
    DLOG_RAW("Unknown command\n");
    return;
  }

  for (const auto& input_command : commands_) {
    if (input_command.GetCategory() == category &&
        input_command.GetCommand() == command) {
      input_command.Run(command_input);
    }
  }


}

template<>
void
InputHandler<Side::kClient>::RunCommand([
  [maybe_unused]] const alflib::String& command)
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
InputHandler<Side::kClient>::Update() {}
}

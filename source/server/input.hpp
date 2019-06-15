#ifndef INPUT_HPP_
#define INPUT_HPP_

#include <alflib/string.hpp>
#include <functional>
#include <future>
#include <game/world.hpp>

namespace dib
{

alflib::String
GetLine();

// ============================================================ //

class InputCommand
{
public:
  enum class Category
  {
    kUnknown,
    kNone,
    kInfo,
    kChat
  };

  InputCommand(const Category category,
               const alflib::String& command,
               std::function<void(alflib::String)> callback);

  static alflib::String CategoryToString(const Category category);

  static Category StringToCategory(const alflib::String& string);

  alflib::String GetCategoryAsString() const
  {
    return CategoryToString(category_);
  }

  Category GetCategory() const { return category_; }

  const alflib::String& GetCommand() const { return command_; }

  void Run(const alflib::String& callback_input) const { callback_(callback_input); }

private:
  Category category_;
  alflib::String command_;
  std::function<void(alflib::String)> callback_;
};

// ============================================================ //

template <Side side>
class InputHandler
{
 public:
  InputHandler();

  void AddCommand(const InputCommand::Category category,
                  const alflib::String& command,
                  std::function<void(alflib::String)> callback);

  void Update();

 private:
  void RunCommand(const alflib::String& input);

 private:
  std::vector<InputCommand> commands_{};
  std::future<alflib::String> input_{};
};
}

#endif//INPUT_HPP_

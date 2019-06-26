#pragma once

// ========================================================================== //
// Macro Declarations
// ========================================================================== //

/** Macro for marking a value as unused **/
#define DIB_UNUSED(value) (void)value

// -------------------------------------------------------------------------- //

/** Macro for forward-declaring a simple class **/
#define DIB_FORWARD_DECLARE_CLASS(cls) class cls

// -------------------------------------------------------------------------- //

/** Macro for forward-declaring a simple class **/
#define DIB_FORWARD_DECLARE_ENUM_CLASS(cls, type) enum class cls : type

// -------------------------------------------------------------------------- //

/** Macro for generating operators for an enum class **/
#define DIB_ENUM_CLASS_OPERATORS(prefix, type, underlying_type)                \
  prefix type operator|(type lhs, type rhs)                                    \
  {                                                                            \
    return static_cast<type>(static_cast<underlying_type>(lhs) |               \
                             static_cast<underlying_type>(rhs));               \
  }                                                                            \
  prefix type operator&(type lhs, type rhs)                                    \
  {                                                                            \
    return static_cast<type>(static_cast<underlying_type>(lhs) &               \
                             static_cast<underlying_type>(rhs));               \
  }

// -------------------------------------------------------------------------- //

/** Macro for declaring a class non-copyable **/
#define DIB_CLASS_NON_COPYABLE(type)                                           \
  type(const type&) = delete;                                                  \
  type& operator=(const type&) = delete;

// -------------------------------------------------------------------------- //

/** Macro for declaring a class default-movable **/
#define DIB_CLASS_DEFAULT_MOVABLE(type)                                        \
  type(type&&) = default;                                                      \
  type& operator=(type&&) = default;

#include "script/expose/expose_mod.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::script {

void
ExposeMod(Environment& environment)
{
  JsValueRef output;
  environment.RunScript("class Mod\n"
                        "{\n"
                        "  init() {}\n"
                        "  update(delta) {}\n"
                        "}\n",
                        output);
}

}
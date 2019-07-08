#include "game/mod/mono_context.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <dlog.hpp>
#include <mono/metadata/assembly.h>

// ========================================================================== //
// Private Functions
// ========================================================================== //

extern "C" void
ScriptLogVerbose(const char* message)
{
  DLOG_VERBOSE(message);
}

// ========================================================================== //
// MonoContext Implementation
// ========================================================================== //

namespace dib::game {

MonoContext::~MonoContext()
{
  mono_jit_cleanup(mDomain);
}

// -------------------------------------------------------------------------- //

MonoContext::MonoContext()
{
  mDomain = mono_jit_init("Diabas");

  // Load core assembly
  mCoreAssembly = mono_domain_assembly_open(mDomain, "./dotnet/diabas.dll");
  AlfAssert(mCoreAssembly != nullptr, "Failed to find 'diabas.dll' assembly");
  mCoreImage = mono_assembly_get_image(mCoreAssembly);
  AlfAssert(mCoreImage != nullptr, "Failed get 'diabas.dll' assembly image");

  // Mod
  mClassMod = mono_class_from_name(mCoreImage, "Diabas", "Mod");
  AlfAssert(mClassMod, "Failed to find class 'Diabas.Mod' in 'diabas.dll'");

  mFieldModId = mono_class_get_field_from_name(mClassMod, "mId");
  AlfAssert(mFieldModId, "Failed to find field 'mId' in class 'Mod'");
  mFieldModName = mono_class_get_field_from_name(mClassMod, "mName");
  AlfAssert(mFieldModName, "Failed to find field 'mName' in class 'Mod'");

  // Util
  mClassUtil = mono_class_from_name(mCoreImage, "Diabas", "Util");
  AlfAssert(mClassUtil, "Failed to find class 'Diabas.Util' in 'diabas.dll'");

  mMethodGetAttributes =
    mono_class_get_method_from_name(mClassUtil, "GetAttributes", 1);
  AlfAssert(mMethodGetAttributes,
            "Failed to find method 'GetAttributes' in class 'Util'");
}

// -------------------------------------------------------------------------- //

std::vector<MonoClass*>
MonoContext::ListClassesInAssembly(MonoAssembly* assembly)
{
  std::vector<MonoClass*> list;

  // Get image from assembly
  MonoImage* image = mono_assembly_get_image(assembly);
  if (!image) {
    return list;
  }

  // Find classes from table in image
  const MonoTableInfo* tableInfo =
    mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
  int rows = mono_table_info_get_rows(tableInfo);
  for (int i = 0; i < rows; i++) {
    u32 columns[MONO_TYPEDEF_SIZE];
    mono_metadata_decode_row(tableInfo, i, columns, MONO_TYPEDEF_SIZE);
    const char* name =
      mono_metadata_string_heap(image, columns[MONO_TYPEDEF_NAME]);
    const char* nameSpace =
      mono_metadata_string_heap(image, columns[MONO_TYPEDEF_NAMESPACE]);
    MonoClass* cls = mono_class_from_name(image, nameSpace, name);
    if (cls) {
      list.push_back(cls);
    }
  }

  return list;
}

// -------------------------------------------------------------------------- //

std::vector<MonoObject*>
MonoContext::ListAttributes(MonoType* type)
{
  std::vector<MonoObject*> list;

  MonoReflectionType* reflType = mono_type_get_object(Instance().mDomain, type);
  void* params[1];
  params[0] = reflType;
  MonoObject* result = mono_runtime_invoke(
    Instance().mMethodGetAttributes, nullptr, params, nullptr);
  MonoArray* attributeArray = (MonoArray*)result;
  for (u64 i = 0; i < mono_array_length(attributeArray); i++) {
    MonoObject* object = mono_array_get(attributeArray, MonoObject*, i);
    list.push_back(object);
  }

  return list;
}

// -------------------------------------------------------------------------- //

MonoObject*
MonoContext::ModAttribute(MonoClass* cls)
{
  // Retrieve all attributes
  MonoType* type = mono_class_get_type(cls);
  std::vector<MonoObject*> attributes = ListAttributes(type);

  // Check if any attribute is a mod
  for (MonoObject* attribute : attributes) {
    MonoClass* attributeClass = mono_object_get_class(attribute);
    if (attributeClass == Instance().mClassMod) {
      return attribute;
    }
  }
  return nullptr;
}

// -------------------------------------------------------------------------- //

MonoContext&
MonoContext::Instance()
{
  static MonoContext instance;
  return instance;
}

}
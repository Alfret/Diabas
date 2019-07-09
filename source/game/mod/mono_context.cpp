#include "game/mod/mono_context.hpp"
#include "mono_context.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <dlog.hpp>
#include <mono/metadata/assembly.h>
#include <mono/metadata/reflection.h>

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

  // Util
  mUtil.classUtil = mono_class_from_name(mCoreImage, "Diabas", "Util");
  AlfAssert(mUtil.classUtil,
            "Failed to find class 'Diabas.Util' in 'diabas.dll'");
  mUtil.methodGetAttributes =
    mono_class_get_method_from_name(mUtil.classUtil, "GetAttributes", 1);
  AlfAssert(mUtil.classUtil,
            "Failed to find method 'GetAttributes' in class 'Util'");

  // Mod
  mMod.classMod = mono_class_from_name(mCoreImage, "Diabas.Attribute", "Mod");
  AlfAssert(mMod.classMod,
            "Failed to find class 'Diabas.Attribute.Mod' in 'diabas.dll'");
  mMod.classModId =
    mono_class_from_name(mCoreImage, "Diabas.Attribute", "ModId");
  AlfAssert(mMod.classModId,
            "Failed to find class 'Diabas.Attribute.ModId' in 'diabas.dll'");
  mMod.classModName =
    mono_class_from_name(mCoreImage, "Diabas.Attribute", "ModName");
  AlfAssert(mMod.classModName,
            "Failed to find class 'Diabas.Attribute.ModName' in 'diabas.dll'");
  mMod.classModRegisterTiles =
    mono_class_from_name(mCoreImage, "Diabas.Attribute", "ModRegisterTile");
  AlfAssert(
    mMod.classModRegisterTiles,
    "Failed to find class 'Diabas.Attribute.ModRegisterTiles' in 'diabas.dll'");
  mMod.classModUpdate =
    mono_class_from_name(mCoreImage, "Diabas.Attribute", "ModUpdate");
  AlfAssert(
    mMod.classModUpdate,
    "Failed to find class 'Diabas.Attribute.ModUpdate' in 'diabas.dll'");

  // Tile
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

std::vector<MonoClassField*>
MonoContext::ListFieldsInClass(MonoClass* cls)
{
  std::vector<MonoClassField*> list;
  void* iter;
  for (int i = 0; i < mono_class_num_fields(cls); i++) {
    MonoClassField* field = mono_class_get_fields(cls, &iter);
    list.push_back(field);
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
    Instance().mUtil.methodGetAttributes, nullptr, params, nullptr);
  MonoArray* attributeArray = (MonoArray*)result;
  for (u64 i = 0; i < mono_array_length(attributeArray); i++) {
    MonoObject* object = mono_array_get(attributeArray, MonoObject*, i);
    list.push_back(object);
  }

  return list;
}

// -------------------------------------------------------------------------- //

MonoObject*
MonoContext::GetClassAttribute(MonoClass* cls, MonoClass* attributeClass)
{
  // Retrieve all attributes
  MonoType* type = mono_class_get_type(cls);
  std::vector<MonoObject*> attributes = ListAttributes(type);

  // Return any matching attribute object
  for (MonoObject* attribute : attributes) {
    MonoClass* _attributeClass = mono_object_get_class(attribute);
    if (_attributeClass == attributeClass) {
      return attribute;
    }
  }
  return nullptr;
}

// -------------------------------------------------------------------------- //

MonoObject*
MonoContext::GetFieldAttribute(MonoClass* cls,
                               MonoClassField* field,
                               MonoClass* attributeClass)
{

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
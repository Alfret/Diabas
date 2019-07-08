#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <vector>
#include <mono/jit/jit.h>

#include "core/types.hpp"

// ========================================================================== //
// MonoContext Declaration
// ========================================================================== //

namespace dib::game {

class MonoContext
{
private:
  /** Mono domain **/
  MonoDomain* mDomain;

  /** Core assembly **/
  MonoAssembly* mCoreAssembly;
  /** Core assembly image **/
  MonoImage* mCoreImage;

  /** 'Mod' class from core assembly **/
  MonoClass* mClassMod;
  /** 'mId' field of 'Mod' class **/
  MonoClassField* mFieldModId;
  /** 'mName' field of 'Mod' class **/
  MonoClassField* mFieldModName;

  /** 'Util' class from core assembly  **/
  MonoClass* mClassUtil;
  /** 'GetAttributes' method from 'Util' class **/
  MonoMethod* mMethodGetAttributes;

public:
  /** Destruct context **/
  ~MonoContext();

private:
  /** Context **/
  MonoContext();

public:
  /** Returns the mono domain **/
  static MonoDomain* GetDomain() { return Instance().mDomain; }

  /** Returns a list of all classes in an assembly **/
  static std::vector<MonoClass*> ListClassesInAssembly(MonoAssembly* assembly);

  /** Returns a list of all attributes for a type **/
  static std::vector<MonoObject*> ListAttributes(MonoType* type);

  /** Returns the mod attribute of a class. Or null if the class does not have
   * one **/
  static MonoObject* ModAttribute(MonoClass* cls);

  /** Returns the field 'Diabas.Mod.mId' **/
  static MonoClassField* GetFieldModId() { return Instance().mFieldModId; }

  /** Returns the field 'Diabas.Mod.mName' **/
  static MonoClassField* GetFieldModName() { return Instance().mFieldModName; }

private:
  /** Returns the singleton instance **/
  static MonoContext& Instance();
};

}
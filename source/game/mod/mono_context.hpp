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

  /** 'Util' related objects **/
  struct
  {
    /** 'Util' class from core assembly  **/
    MonoClass* classUtil;
    /** 'GetAttributes' method from 'Util' class **/
    MonoMethod* methodGetAttributes;
  } mUtil;

  /** 'Mod' related objects **/
  struct
  {
    /** 'Mod' class from core assembly **/
    MonoClass* classMod;
    /** 'ModId' class from core assembly **/
    MonoClass* classModId;
    /** 'ModName' class from core assembly **/
    MonoClass* classModName;
    /** 'ModRegisterTiles' class from core assembly **/
    MonoClass* classModRegisterTiles;
    /** 'Mod' class from core assembly **/
    MonoClass* classModUpdate;
  } mMod;

  /** 'Tile' and 'TileRegistry' related objects **/
  struct
  {

  } mTile;

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

  /** Returns a list of fields in a class **/
  static std::vector<MonoClassField*> ListFieldsInClass(MonoClass* cls);

  /** Returns a list of all attributes for a type **/
  static std::vector<MonoObject*> ListAttributes(MonoType* type);

  /** Returns the attribute of a class. Or null if the class does not have
   * that attribute **/
  static MonoObject* GetClassAttribute(MonoClass* cls,
                                       MonoClass* attributeClass);

  /** Returns the attribute of a field. Or null if the field does not have
   * that attribute **/
  static MonoObject* GetFieldAttribute(MonoClass* cls,
                                       MonoClassField* field,
                                       MonoClass* attributeClass);

  /** Returns the 'Mod' class **/
  static MonoClass* GetModClass() { return Instance().mMod.classMod; }

  /** Returns the 'ModId' class **/
  static MonoClass* GetModIdClass() { return Instance().mMod.classModId; }

  /** Returns the 'ModName' class **/
  static MonoClass* GetModNameClass() { return Instance().mMod.classModName; }

private:
  /** Returns the singleton instance **/
  static MonoContext& Instance();
};

}
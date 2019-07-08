using System;
using Diabas;

namespace Farming {

[Mod]
public class FarmingMod
{

  [ModId]
  private string mId;
  [ModName]
  private string mName;
  
  public FarmingMod()
  {
    Diabas.Log.Verbose("Constructing farming mod");
  }

  [ModUpdate]
  public void Update(double delta)
  {

  }

}

}

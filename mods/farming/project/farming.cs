using System;
using Diabas;

namespace Farming {

[Diabas.Attribute.Mod]
public class FarmingMod
{

  [Diabas.Attribute.ModId]
  private string mId;
  [Diabas.Attribute.ModName]
  private string mName;

  private Tile mTileCrate;
  
  public FarmingMod()
  {
    Diabas.Log.Verbose($"Constructing {mName} mod");
    //mTileCrate = new Tile(new ResourcePath(mId, "res/crate.tga"), "crate");
  }

  [Diabas.Attribute.ModRegisterTile]
  public void RegisterTiles(TileRegistry registry)
  {
    Diabas.Log.Verbose("Registering tiles for Farming mod");
    //registry.RegisterTile(mTileCrate, "crate");
  }
  

  [Diabas.Attribute.ModUpdate]
  public void Update(double delta)
  {
  }

}

}

export default class FarmingMod extends Mod {

  constructor() {
    super();
    dlog.verbose("FarmingMod::constructor()");
  }

  init() {
    this.tileCrate = new Tile(this, "res/crate.tga");
  }

  onRegisterTiles(tileManager) {
    dlog.verbose("FarmingMod::onRegisterTiles()");

    tileManager.registerTile("crate", this.tileCrate);
  }

}

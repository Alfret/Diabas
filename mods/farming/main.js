export default class FarmingMod extends Mod {

  constructor() {
    super();
    dlog.verbose("FarmingMod::constructor()");
  }

  init() {
    this.tileCrate = new Tile(this, "res/crate.tga");
    this.tilePurpleFlower = new Tile(this, "res/purple_flower.tga");
  }

  onRegisterTiles(tileManager) {
    dlog.verbose("FarmingMod::onRegisterTiles()");

    tileManager.registerTile("crate", this.tileCrate);
    tileManager.registerTile("purple_flower", this.tilePurpleFlower);
  }

}

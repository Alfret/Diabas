export default class FarmingMod extends Mod {

  constructor() {
    super();
    dlog.verbose("FarmingMod::constructor()");
  }

  init() {
    this.tileCrate = new Tile(this, "res/crate.tga", "crate");
    this.tileGhostDirt = new Tile(this, "res/ghost_dirt.tga", "ghost_dirt");

    this.registerTiles("crate", this.tileCrate, "ghost_dirt", this.tileGhostDirt);
  }

}

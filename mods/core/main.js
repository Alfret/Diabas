export default class CoreMod extends Mod {

  constructor() {
    super();
    dlog.verbose("CoreMod::constructor()");
  }

  init() {
    this.tileDirt = new Tile(this, "res/dirt.png", "dirt");
    this.tileGrass = new Tile(this, "res/grass_top.png", "grass");
    this.tileRock = new Tile(this, "res/rock.png", "rock");

    this.registerTiles(
      "dirt", this.tileDirt,
      "grass", this.tileGrass,
      "rock", this.tileRock);

  }


}

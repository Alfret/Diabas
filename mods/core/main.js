import {my_add} from './util.js';

export default class CoreMod extends Mod {

  constructor() {
    super();
    dlog.verbose("CoreMod::constructor()");
  }

  init() {
    this.tileDirt = new Tile(this, "res/dirt.png");
    this.tileGrass = new Tile(this, "res/grass_top.png");
    this.tileRock = new Tile(this, "res/rock.png");
    this.tileFurnace = new Tile(this, "res/furnace.png");
    this.tileFlowerDaisyBlue = new Tile(this, "res/flower_daisy_blue.tga");
    this.tileFlowerDaisyPurple = new Tile(this, "res/flower_daisy_purple.tga");
    this.tileFlowerDaisyWhite = new Tile(this, "res/flower_daisy_white.tga");
    this.tileSunflowerYellow = new Tile(this, "res/sunflower_yellow.tga");
  }

  onRegisterTiles(tileManager) {
    dlog.verbose("CoreMod::onRegisterTiles()");

    tileManager.registerTile("dirt", this.tileDirt);
    tileManager.registerTile("grass", this.tileGrass);
    tileManager.registerTile("rock", this.tileRock);
    tileManager.registerTile("furnace", this.tileFurnace);
    tileManager.registerTile("flower daisy blue", this.tileFlowerDaisyBlue);
    tileManager.registerTile("flower daisy purple", this.tileFlowerDaisyPurple);
    tileManager.registerTile("flower daisy white", this.tileFlowerDaisyWhite);
  }

}

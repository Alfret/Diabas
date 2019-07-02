import {my_add} from './util.js';

export default class CoreMod extends Mod {

  constructor() {
    super();
    dlog.verbose("CoreMod::constructor()");
  }

  init() {
    this.tileDirt = new Tile(this, "res/dirt.tga");
    this.tileGrass = new Tile(this, "res/grass_top.tga");
    this.tileGrassTopLeft = new Tile(this, "res/grass_top_left.tga");
    this.tileGrassTopRight = new Tile(this, "res/grass_top_right.tga");
    this.tileGrassTopLeftRight = new Tile(this, "res/grass_top_left_right.tga");
    this.tileRock = new Tile(this, "res/rock.tga");
    this.tileFurnace = new Tile(this, "res/furnace.tga");
    this.tileFlowerDaisyBlue = new Tile(this, "res/flower_daisy_blue.tga");
    this.tileFlowerDaisyPurple = new Tile(this, "res/flower_daisy_purple.tga");
    this.tileFlowerDaisyWhite = new Tile(this, "res/flower_daisy_white.tga");
    this.tileSunflowerYellow = new Tile(this, "res/sunflower_yellow.tga");
    this.tileSlime = new Tile(this, "res/slime.tga");
  }

  onRegisterTiles(tileManager) {
    dlog.verbose("CoreMod::onRegisterTiles()");

    tileManager.registerTile("dirt", this.tileDirt);
    tileManager.registerTile("grass", this.tileGrass);
    tileManager.registerTile("grass top left", this.tileGrassTopLeft);
    tileManager.registerTile("grass top right", this.tileGrassTopRight);
    tileManager.registerTile("grass top left right", this.tileGrassTopLeftRight);
    tileManager.registerTile("rock", this.tileRock);
    tileManager.registerTile("furnace", this.tileFurnace);
    tileManager.registerTile("flower daisy blue", this.tileFlowerDaisyBlue);
    tileManager.registerTile("flower daisy purple", this.tileFlowerDaisyPurple);
    tileManager.registerTile("flower daisy white", this.tileFlowerDaisyWhite);
    tileManager.registerTile("sunflower yellow", this.tileSunflowerYellow);
    tileManager.registerTile("slime", this.tileSlime);
  }

}

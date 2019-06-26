import {my_add} from './util.js';

export default class CoreMod extends Mod {

  constructor() {
    super();
    dlog.verbose("CoreMod::constructor()");
  }

  init() {
    this.tileDirt = new Tile(this, "res/dirt.png");
    this.tileRock = new Tile(this, "res/rock.png");
    this.tileFurnace = new Tile(this, "res/furnace.png");
  }

  onRegisterTiles(tileManager) {
    dlog.verbose("CoreMod::onRegisterTiles()");

    tileManager.registerTile("dirt", this.tileDirt);
    tileManager.registerTile("rock", this.tileRock);
    tileManager.registerTile("furnace", this.tileFurnace);

  }

}

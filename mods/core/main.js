export default class CoreMod extends Mod {

  /** Called when mod is instantiated. Not all information and functions
   * are available to the mod in this function, as the game engine has
   * not yet injected that. */
  constructor() {
    super();
    dlog.verbose("CoreMod::constructor()");
  }

  /** Called to let the mod do all initialization. At this point all the
   * fields of the mod has been initialized and can be used in operations.
   * Some of these are 'name' and 'id'.
   *
   * At this point all 'Tile' objects that the mod is adding MUST be
   * registered.
   *
   * At this point all 'Item' objects that the mod is adding MUST be
   * registered.
   */
  init() {
    this.tileDirt = new Tile(this, "res/dirt.png", "dirt");
    this.tileGrass = new Tile(this, "res/grass_top.png", "grass");
    this.tileRock = new Tile(this, "res/rock.png", "rock");

    this.registerTiles(
      "dirt", this.tileDirt,
      "grass", this.tileGrass,
      "rock", this.tileRock);
  }

  /** Called by the engine to let the modification do any custom world
   * generation. The world to apply the generation to and the phase
   * that is currently run, is available in the arguments.
   *
   * The phase can be used to control when different parts of the
   * world should be generated.
   *
   * Phases include:
   * - Initial: Initial bulk generation of the game world. Here the
   *   different layers and biome-specific blocks should be generated.
   * - Ore: During this phase ores and gems should be generated.
   * - Structure: During this phase structures should be generated.
   * - Chest/Loot: During this phase any chest and other types of
   *   dungeon loot should be generated.
   */
  generateWorld(world, phase) {

  }


}

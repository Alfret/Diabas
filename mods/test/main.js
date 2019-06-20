export default class TestMod extends Mod {

  constructor() {
    super();
    dlog.info("TestMod::constructor()");
  }

  init() {
    dlog.verbose("TestMod::init()");
  }

  update(delta) {
    //dlog.verbose("TestMod::update(", delta, ")");
  }

}
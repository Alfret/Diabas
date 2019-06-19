import {my_add} from './util.js';

export default class CoreMod extends Mod {

  constructor() {
    super();
    dlog.info("CoreMod::constructor()");
  }

  init() {
    dlog.verbose("CoreMod::init()");
    dlog.info("Hello from mod ", this.name, "!");
  }

  update(delta) {
    //dlog.verbose("CoreMod::update(", delta, ")");
  }

}
import { my_add } from './util.js';

class CoreMod extends Mod {

    constructor() {
        super();
        dlog.info("info: ", 22, "other", true, 3.14, "HELLO");
        dlog.info("Added: 2 + 3 = ", my_add(2, 3));
    }

    init() {
        dlog.verbose("CoreMod::init");
    }

    update(delta) {
      dlog.verbose("Delta: " + delta);
    }

}
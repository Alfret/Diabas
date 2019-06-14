class CoreMod extends Mod {

    constructor() {
        super();
        dlog.info("info: ", 22, "other", true, 3.14, "HELLO");
    }

    init() {
        dlog.verbose("CoreMod::init");
    }

    update(delta) {
      dlog.verbose("Delta: " + delta);
    }

}
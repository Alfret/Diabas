import {my_add} from './util.js';

export default class CoreMod extends Mod {

    constructor() {
        super();
        dlog.info("Constructing CoreMod (", my_add(2, 3), ")");
        this.name = "Hej";
        this.age = 33;
    }

    j

    init() {
        dlog.verbose("CoreMod::init");
    }

    update(delta) {
        dlog.verbose("Delta: " + delta);
    }

}
import {my_add} from './util.js';

export default class CoreMod extends Mod {

  constructor() {
    super();
    dlog.info("CoreMod::constructor()");
  }

  init() {
    dlog.verbose("CoreMod::init()");
    //dlog.info("Hello from mod ", this.name, "(", this.id, ")!");

    this.registerPacketType("spawn_particle");
    this.registerPacketType("spawn_crocodile");

    let p = new Packet("spawn_particle");
    p.write(2);
    p.write("Hej");
    this.sendPacket(p);

  }

  update(delta) {
    //dlog.verbose("CoreMod::update(", delta, ")");
  }

  onPacketReceived(p) {
    //let i = p.read();
  }

}
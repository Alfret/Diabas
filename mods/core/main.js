import {my_add} from './util.js';

export default class CoreMod extends Mod {

  constructor() {
    super();
    dlog.info("CoreMod::constructor()");
  }

  init() {
    this.registerPacketType("my_packet");
  }

  onKeyPress(key) {
    if (key === keys.S && this.isClient()) {
      let p = new Packet("my_packet");
      p.write("Data in my packet");
      this.sendPacket(p);
    }
  }

  onPacketReceived(packet) {
    dlog.verbose("Packet received of type ", packet.getType());
    if (packet.getType() === "my_packet") {
      let data = packet.read();
      dlog.verbose("Data: ", data);
    }
  }


}

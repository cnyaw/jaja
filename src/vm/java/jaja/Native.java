package jaja;

import java.util.*;

class Native {

  //
  // Native call name.
  //

  static final int NATIVEsystem_outchr = 0;
  static final int NATIVEsystem_outint = 1;
  static final int NATIVEsystem_outstr = 2;
  static final int NATIVEsystem_readint = 3;

  Vector api = new Vector();

  public Native() {
    api.addElement("system_outchr");
    api.addElement("system_outint");
    api.addElement("system_outstr");
    api.addElement("system_readint");
  }

  //
  // Native call.
  //

  static void nativeCall(jaja aJaja, int iNative) {
    switch (iNative) {
      case NATIVEsystem_outchr:
        System.out.print((char)(aJaja.stack[aJaja.sp++] & 0xff));
        break;
      case NATIVEsystem_outint:
        System.out.print(aJaja.stack[aJaja.sp++]);
        break;
      case NATIVEsystem_outstr: {
          int addr = aJaja.stack[aJaja.sp++];
          if (jaja.MAX_STACK + jaja.MAX_STACK2 <= addr) { // In heap.
            int tail = addr;
            for (; 0 != aJaja.heap[tail]; tail++) {}
            System.out.print(new String(aJaja.heap, addr, tail - addr));
          }
          else {                        // In stack.
          }
        }
        break;
      case NATIVEsystem_readint:
        aJaja.stack[--aJaja.sp] = -1;
        break;
    }
  }
}

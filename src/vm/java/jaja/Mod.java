package jaja;

import java.util.Vector;

class Mod {
  static final int TAG_NAME = 0x19;
  static final int TAG_EXPORT = 0x80;
  static final int TAG_IMPORT = 0x12;
  static final int TAG_CODE = 0x13;
  static final int TAG_DONE = 0xcd;
  static final int TAG_NATIVE = 0xffff;

  int iMod;
  String modName;
  int offset;
  int size;
  Vector names = new Vector();
  Vector exports = new Vector();
  Vector imports = new Vector();
}

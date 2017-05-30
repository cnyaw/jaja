package jaja;

import java.io.*;
import java.util.*;

class Loader {

  //
  // return -1 if load failed, otherwise return new loaded MOD handle/index.
  //

  static int loadMod(jaja aJaja, String modName) {

    FileInputStream fis = null;
    try {
      fis = new FileInputStream(modName + ".m");
    }
    catch (FileNotFoundException e) {
      System.out.println("Can not open MOD file '" + modName + "'");
      return -1;
    }
    catch (Exception e) {
      return -1;
    }

    byte stream[] = null;
    try {
      stream = new byte[fis.available()];
    }
    catch (Exception e) {
      return -1;
    }

    try {
      fis.read(stream, 0, stream.length);
    }
    catch (IOException e) {
      return -1;
    }

    return loadMod(aJaja, modName, stream);
  }

  static int loadMod(jaja aJaja, String modName, byte stream[]) {

    //
    // Check is already loaded?
    //

    for (int i = 0; i < aJaja.mods.size(); i++)
      if (((Mod)aJaja.mods.elementAt(i)).modName.equals(modName))
        return i;

    //
    // Skip dummy bytes.
    //

    int p = 16;

    //
    // Check magic ID.
    //

    String magic = new String(stream, p, 4);
    if (!magic.equals("jaja")) {
      System.out.println("Not a MOD file '" + modName + "'xxx");
      return -1;
    }

    p += 4;

    //
    // Check version.
    //

    if (stream[p++] != 0 || stream[p++] != 1) {
      System.out.println("Unknown version MOD file '" + modName + "'");
      return -1;
    }

    //
    // Read sections.
    //

    Mod aMod = new Mod();
    aMod.modName = modName;
    aMod.size = 0;
    aMod.offset = -1;

    boolean bDone = false;
    while (!bDone) {
      int tag = stream[p++] & 0xff ;
      switch (tag) {
        case Mod.TAG_NAME: {            // Name.
          int nItem = (stream[p++] & 0xff) | ((stream[p++] & 0xff) << 8);
          for (int i = 0; i < nItem; i++) {
            int len = (stream[p++] & 0xff) | ((stream[p++] & 0xff) << 8);
            String name = new String(stream, p, len);
            p += len;
            aMod.names.addElement(name);
          }
        }
        break;
        case Mod.TAG_EXPORT: {          // Export.
          int nItem = (stream[p++] & 0xff) | ((stream[p++] & 0xff) << 8);
          for (int i = 0; i < nItem; i++) {
            Export e = new Export();
            e.iLabelName = (stream[p++] & 0xff) | ((stream[p++] & 0xff) << 8);
            e.offset = (stream[p++] & 0xff) | ((stream[p++] & 0xff) << 8);
            e.iNative = -1;
            if (Mod.TAG_NATIVE == (e.offset & 0xffff)) {
              String nc = modName + "_" + (String)aMod.names.elementAt(e.iLabelName);
              for (int j = 0; j < aJaja.natives.api.size(); j++)
                if (((String)aJaja.natives.api.elementAt(j)).equals(nc)) {
                  e.iNative = j;
                  break;
                }
              if (-1 == e.iNative) {
                System.out.println("Unresolved native call '" + nc + " '" + modName + "'");
                return -1;
              }
            }
            aMod.exports.addElement(e);
          }
        }
        break;
        case Mod.TAG_IMPORT: {          // Import.
          int nItem = (stream[p++] & 0xff) | ((stream[p++] & 0xff) << 8);
          for (int i = 0; i < nItem; i++) {
            Import ip = new Import();
            ip.iModName = (stream[p++] & 0xff) | ((stream[p++] & 0xff) << 8);
            ip.iLabelName = (stream[p++] & 0xff) | ((stream[p++] & 0xff) << 8);
            ip.iMod = -1;
            ip.iNative = -1;
            aMod.imports.addElement(ip);
          }
        }
        break;
        case Mod.TAG_CODE: {            // Code.
          aMod.size = (stream[p++] & 0xff) | ((stream[p++] & 0xff) << 8);
          while (aJaja.heap.length < aJaja.hp + aMod.size) {
            byte newHeap[] = new byte[2 * aJaja.heap.length];
            System.arraycopy(aJaja.heap, 0, newHeap, 0, aJaja.heap.length);
            aJaja.heap = newHeap;
          }
          System.arraycopy(stream, p, aJaja.heap, aJaja.hp, aMod.size);
          aMod.offset = aJaja.hp;
          aJaja.hp += aMod.size;
          p += aMod.size;
        }
        break;
        case Mod.TAG_DONE:              // Done.
          bDone = true;
          System.out.println("MOD '" + modName + "' loaded");
          break;
        default:                        // Invalid.
          System.out.println((tag & 0xff) + ": Unknown section tag read '" + modName + "'");
          return -1;
      }
    }

    aJaja.mods.addElement(aMod);
    return aJaja.mods.size() - 1;
  }
}

//
// jaja vm, java reference implementation.
//

package jaja;

import java.io.*;
import java.util.*;

public class jaja {

  // const
  static final int MAX_STACK = 512;
  static final int MAX_STACK2 = 512;
  static final int DEF_HEAP = 2 * (MAX_STACK + MAX_STACK2);

  // opcode
  static final int OP_NOP = 0;
  static final int OP_PUSH1 = 1;
  static final int OP_PUSH2 = 2;
  static final int OP_PUSH4 = 3;
  static final int OP_PUSHLABEL = 4;
  static final int OP_PUSHMOD = 5;
  static final int OP_DISCARD = 6;
  static final int OP_DISCARDX = 7;
  static final int OP_DUP = 8;
  static final int OP_SWAP = 9;
  static final int OP_LOAD1 = 10;
  static final int OP_LOAD2 = 11;
  static final int OP_LOAD4 = 12;
  static final int OP_SAVE1 = 13;
  static final int OP_SAVE2 = 14;
  static final int OP_SAVE4 = 15;
  static final int OP_PARAM = 16;
  static final int OP_LOCAL = 17;
  static final int OP_BOOK = 18;
  static final int OP_FREE = 19;
  static final int OP_LOADPARAM = 20;
  static final int OP_SAVEPARAM = 21;
  static final int OP_LOADLOCAL = 22;
  static final int OP_SAVELOCAL = 23;
  static final int OP_ADD = 24;
  static final int OP_SUB = 25;
  static final int OP_MUL = 26;
  static final int OP_DIV = 27;
  static final int OP_MOD = 28;
  static final int OP_INC = 29;
  static final int OP_DEC = 30;
  static final int OP_INCPARAM = 31;
  static final int OP_DECPARAM = 32;
  static final int OP_INCLOCAL = 33;
  static final int OP_DECLOCAL = 34;
  static final int OP_NOT = 35;
  static final int OP_NEG = 36;
  static final int OP_AND = 37;
  static final int OP_OR = 38;
  static final int OP_XOR = 39;
  static final int OP_SHL = 40;
  static final int OP_SHR = 41;
  static final int OP_JZ = 42;
  static final int OP_JNZ = 43;
  static final int OP_JE = 44;
  static final int OP_JNE = 45;
  static final int OP_JL = 46;
  static final int OP_JLE = 47;
  static final int OP_JG = 48;
  static final int OP_JGE = 49;
  static final int OP_JUMP = 50;
  static final int OP_CALL = 51;
  static final int OP_CALLMOD = 52;
  static final int OP_RET = 53;

  // class start here
  Vector mods = new Vector();
  Native natives = new Native();

  byte heap[];
  int hp;

  int stack[];
  int sp, xp;

  int im, cs, ip, pp, lp;

  public jaja() {
    heap = new byte[DEF_HEAP];
    hp = MAX_STACK + MAX_STACK2;
    stack = new int[MAX_STACK / 4 + MAX_STACK2 / 4];
    ip = 0;
    pp = lp = sp = MAX_STACK / 4;
    xp = (MAX_STACK + MAX_STACK2) / 4;
    im = cs = -1;
  }

  public boolean invoke(String modName) {
    int iMod = Loader.loadMod(this, modName);
    if (-1 == iMod) return false;

    stack[--xp] = im;
    stack[--xp] = cs;

    im = iMod;
    cs = ((Mod)mods.elementAt(iMod)).offset;

    stack[--xp] = ip;
    stack[--xp] = -1;                   // Dummy mod call mark.
    stack[--xp] = pp;

    return true;
  }

  public boolean runStep() {
    if (-1 == im)                       // No active mod
      return false;

    int op = heap[cs + ip++] & 0xff, a, b;
    switch (op) {
      case OP_NOP:
        break;
      case OP_PUSH1:
        stack[--sp] = heap[cs + ip++];
        break;
      case OP_PUSH2:
        stack[--sp] = heap[cs + ip++] | (heap[cs + ip++] << 8);
        break;
      case OP_PUSH4:
        stack[--sp] = heap[cs + ip++] | (heap[cs + ip++] << 8) | (heap[cs + ip++] << 16) | (heap[cs + ip++] << 24);
        break;
      case OP_PUSHLABEL:
        stack[--sp] = cs + ((heap[cs + ip++] & 0xff) | ((heap[cs + ip++] & 0xff) << 8));
        break;
      case OP_PUSHMOD:
        break;
      case OP_DISCARD:
        sp++;
        break;
      case OP_DISCARDX:
        sp += stack[sp] + 1;
        break;
      case OP_DUP:
        stack[--sp] = stack[sp + 1];
        break;
      case OP_SWAP: {
        int tmp = stack[sp];
        stack[sp] = stack[sp + 1];
        stack[sp + 1] = tmp;
        }
        break;
      case OP_LOAD1:
        stack[sp] = (int)(heap[stack[sp]] & 0xff);
        break;
      case OP_LOAD2:
        a = stack[sp];
        stack[sp] = (int)((heap[a] & 0xff) | ((heap[a + 1] << 8) & 0xff));
        break;
      case OP_LOAD4:
        a = stack[sp];
        stack[sp] = (int)((heap[a] & 0xff) | ((heap[a + 1] << 8) & 0xff) | ((heap[a + 2] << 16) & 0xff) | ((heap[a + 3] << 24) & 0xff));
        break;
      case OP_SAVE1:
        heap[stack[sp]] = (byte)(stack[sp + 1] & 0xff);
        sp += 2;
        break;
      case OP_SAVE2:
        a = stack[sp];
        b = stack[sp + 1];
        heap[a + 0] = (byte)(b & 0xff);
        heap[a + 1] = (byte)((b >> 8) & 0xff);
        sp += 2;
        break;
      case OP_SAVE4:
        a = stack[sp];
        b = stack[sp + 1];
        heap[a + 0] = (byte)(b & 0xff);
        heap[a + 1] = (byte)((b >> 8) & 0xff);
        heap[a + 2] = (byte)((b >> 16) & 0xff);
        heap[a + 3] = (byte)((b >> 24) & 0xff);
        sp += 2;
        break;
      case OP_PARAM:
        stack[sp] += pp;
        break;
      case OP_LOCAL:
        stack[sp] += lp;
        break;
      case OP_BOOK:
        sp -= stack[sp] - 1;
        stack[--xp] = lp;
        lp = sp;
        break;
      case OP_FREE:
        sp += stack[sp] + 1;
        lp = stack[xp++];
        break;
      case OP_LOADPARAM:
        stack[sp] = stack[pp + stack[sp]];
        break;
      case OP_SAVEPARAM:
        stack[pp + stack[sp]] = stack[sp + 1];
        sp += 2;
        break;
      case OP_LOADLOCAL:
        stack[sp] = stack[lp + stack[sp]];
        break;
      case OP_SAVELOCAL:
        stack[lp + stack[sp]] = stack[sp + 1];
        sp += 2;
        break;
      case OP_ADD:
        stack[sp + 1] += stack[sp++];
        break;
      case OP_SUB:
        stack[sp + 1] -= stack[sp++];
        break;
      case OP_MUL:
        stack[sp + 1] *= stack[sp++];
        break;
      case OP_DIV:
        stack[sp + 1] /= stack[sp++];
        break;
      case OP_MOD:
        stack[sp + 1] %= stack[sp++];
        break;
      case OP_INC:
        stack[sp]++;
        break;
      case OP_DEC:
        stack[sp]--;
        break;
      case OP_INCPARAM:
        stack[pp + stack[sp++]]++;
        break;
      case OP_DECPARAM:
        stack[pp + stack[sp++]]--;
        break;
      case OP_INCLOCAL:
        stack[lp + stack[sp++]]++;
        break;
      case OP_DECLOCAL:
        stack[lp + stack[sp++]]--;
        break;
      case OP_NOT:
        stack[sp] = ~ stack[sp];
        break;
      case OP_NEG:
        stack[sp] = - stack[sp];
        break;
      case OP_AND:
        stack[sp + 1] &= stack[sp++];
        break;
      case OP_OR:
        stack[sp + 1] |= stack[sp++];
        break;
      case OP_XOR:
        stack[sp + 1] ^= stack[sp++];
        break;
      case OP_SHL:
        stack[sp] <<= 1;
        break;
      case OP_SHR:
        stack[sp] >>= 1;
        break;
      case OP_JZ:
      case OP_JNZ:
      case OP_JE:
      case OP_JNE:
      case OP_JL:
      case OP_JLE:
      case OP_JG:
      case OP_JGE: {
        int target = heap[cs + ip++] | (heap[cs + ip++] << 8);
        switch (op) {
          case OP_JZ:
            if (0 == stack[sp]) ip = target;
            sp++;
            break;
          case OP_JNZ:
            if (0 != stack[sp]) ip = target;
            sp++;
            break;
          case OP_JE:
            if (stack[sp + 1] == stack[sp]) ip = target;
            sp += 2;
            break;
          case OP_JNE:
            if (stack[sp + 1] != stack[sp]) ip = target;
            sp += 2;
            break;
          case OP_JL:
            if (stack[sp + 1] < stack[sp]) ip = target;
            sp += 2;
            break;
          case OP_JLE:
            if (stack[sp + 1] <= stack[sp]) ip = target;
            sp += 2;
            break;
          case OP_JG:
            if (stack[sp + 1] > stack[sp]) ip = target;
            sp += 2;
            break;
          case OP_JGE:
            if (stack[sp + 1] >= stack[sp]) ip = target;
            sp += 2;
            break;
          }
        }
        break;
      case OP_JUMP:
        ip = heap[cs + ip++] | (heap[cs + ip++] << 8);
        break;
      case OP_CALL:
        stack[--xp] = ip + 2;
        stack[--xp] = pp;
        ip = heap[cs + ip++] | (heap[cs + ip++] << 8);
        pp = sp;
        break;
      case OP_CALLMOD: {
        int iImport = heap[cs + ip++] | (heap[cs + ip++] << 8);

        Mod aMod = (Mod)mods.elementAt(im);
        Import aImport = (Import)aMod.imports.elementAt(iImport);

        if (-1 == aImport.iMod) {
          int iMod = Loader.loadMod(this, (String)aMod.names.elementAt(aImport.iModName));
          if (-1 == iMod) return false;

          String labelName = (String)aMod.names.elementAt(aImport.iLabelName);
          Mod callMod = (Mod)mods.elementAt(iMod);
          for (int i = 0; i < callMod.exports.size(); i++) {
            Export ep = (Export)callMod.exports.elementAt(i);
            String n = (String)callMod.names.elementAt(ep.iLabelName);
            if (labelName.equals(n)) {
              aImport.iMod = iMod;
              aImport.offset = ep.offset;
              aImport.iNative = ep.iNative;
              break;
            }
          }
        }

        if (-1 != aImport.iMod) {
          if (Mod.TAG_NATIVE == (aImport.offset & 0xffff)) {
            natives.nativeCall(this, aImport.iNative);
          }
          else {
            stack[--xp] = im;
            stack[--xp] = cs;
            im = aImport.iMod;
            cs = ((Mod)mods.elementAt(im)).offset;
            stack[--xp] = ip;
            stack[--xp] = -1;
            stack[--xp] = pp;
            ip = aImport.offset;
            pp = sp;
          }
        }
        else {
          System.out.println("unresolved mod call");
          return false;
        }
      }
      break;
    case OP_RET:
      pp = stack[xp++];
      ip = stack[xp++];
      if (-1 == ip) {
        ip = stack[xp++];
        cs = stack[xp++];
        im = stack[xp++];
        if (-1 == im) return false; // finished run
      }
      break;
    default:
      System.out.println("unknown op code: " + op);
      return false;
    }

    return true;
  }
}

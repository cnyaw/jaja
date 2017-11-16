//
// jaja, module format.
//
// 2009/08/14 Waync Cheng
//

#pragma once

// header
#define VER_MAJOR 0x00
#define VER_MINOR 0x01

char HDR_DUMMY[16];
char HDR_TAG[] = {'j', 'a', 'j', 'a'};
char HDR_VER[] = {VER_MAJOR, VER_MINOR};

//
// Tag.
//

enum TAG_
{
  TAG_NAME    = 0x19,
  TAG_EXPORT  = 0x80,
  TAG_IMPORT  = 0x12,
  TAG_CODE    = 0x13,
  TAG_DONE    = 0xcd,
};

//
// MOD FORMAT.
//

// HEADER
//      dummy(16)
//      tag(4)      = "jaja"
//      version(2)  = [VER_MAJOR, VER_MINOR]

// TAG_NAME
//      nItem(2)
//      str(2+n)    = (len(2) str(len)) x nItem

// TAG_EXPORT
//      nItem(2)
//      item(4)     = (idxName(2) offset(2)) x nItem

// TAG_IMPORT
//      nItem(2)
//      item(4)     = (idxName(2) idxName(2)) x nItem

// TAG_CODE
//      len(2)
//      code(len)   = byte code(len)

// end of mod.h

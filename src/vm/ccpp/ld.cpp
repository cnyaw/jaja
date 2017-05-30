//
// jaja, MOD loader.
//
// 2009/08/14 Waync Cheng
//

#include "ld.h"
#include "vm.hxx"

#include "../../mod.h"
#include "../../op.h"

extern VMImpl vm;

int loadMod(const std::string& modName)
{
  //
  // Search if the MOD has already loaded. If the MOD has loaded
  // then returns its ID(index).
  //

  for (int i = 0; i < (int)vm.mod.size(); i++) {
    if (vm.mod[i].modName == modName) {
      return i;
    }
  }

  //
  // Load the module.
  //

  FILE* pFile = fopen((vm.path + modName + ".m").c_str(), "rb");
  if (0 == pFile) {
    printf("Can't open MOD file '%s'\n", modName.c_str());
    return -1;
  }

  fseek(pFile, 0, SEEK_END);
  size_t szStream = ftell(pFile);
  fseek(pFile, 0, SEEK_SET);

  unsigned char* pStream = new unsigned char[szStream];
  fread(pStream, 1, szStream, pFile);
  fclose(pFile);

  int idx = loadMod(modName, szStream, pStream);
  delete [] pStream;

  return idx;
}

int loadMod(const std::string& modName, size_t szStream, const unsigned char* pStream)
{
  //
  // Search if the MOD has already loaded. If the MOD has loaded
  // then returns its ID(index).
  //

  for (int i = 0; i < (int)vm.mod.size(); i++) {
    if (vm.mod[i].modName == modName) {
      return i;
    }
  }

  //
  // Skip dummy bytes.
  //

  int p = sizeof(HDR_DUMMY);

  //
  // Check magic ID.
  //

  if (strncmp((const char*)pStream + p, HDR_TAG, sizeof(HDR_TAG))) {
    printf("Invalid MOD file '%s'\n", modName.c_str());
    return -1;
  }

  p += sizeof(HDR_TAG);

  //
  // Check version.
  //

  if (VER_MAJOR != pStream[p++] || VER_MINOR != pStream[p++]) {
    printf("Unknown version MOD file '%s'\n", modName.c_str());
    return -1;
  }

  //
  // Load sections.
  //

  int a, b;
  VMImpl::Mod mod;
  mod.size = 0;
  mod.offset = -1;

  bool bDone = false;
  while (!bDone) {

    int tag = pStream[p++] & 0xff;
    switch (tag)
    {
    case TAG_NAME:                      // Name table.
      {
        //
        // Get number items.
        //

        a = pStream[p++], b = pStream[p++] << 8;
        int nItem = a | b;

        //
        // Read string items.
        //

        for (int i = 0; i < nItem;++i) {

          //
          // Get string length.
          //

          a = pStream[p++], b = pStream[p++] << 8;
          int len = a | b;

          //
          // Read string.
          //

          mod.names.push_back(std::string((const char*)(pStream + p), len));
          p += len;
        }
      }
      break;

    case TAG_EXPORT:                    // Export table.
      {
        //
        // Get number items.
        //

        a = pStream[p++], b = pStream[p++] << 8;
        int nItem = a | b;

        //
        // Read export items.
        //

        for (int i = 0; i < nItem;++i) {

          //
          // Read item data.
          //

          VMImpl::Export e;
          a = pStream[p++], b = pStream[p++] << 8;
          e.iLabelName = a | b;
          a = pStream[p++], b = pStream[p++] << 8;
          e.offset = a | b;

          //
          // If this is a native export function, then get the native function
          // address and set relative to the export item data.
          //

          if (TAG_NATIVE == e.offset) {
            std::string native = modName + "." + mod.names[e.iLabelName];
            if (vm.native.end() == vm.native.find(native)) {
              printf("Unresolved native call %s '%s.m'\n", native.c_str(), modName.c_str());
              return -1;
            }
            e.native = vm.native[native];
          }

          //
          // Store the export item data.
          //

          mod.exports.push_back(e);
        }
      }
      break;

    case TAG_IMPORT:                    // Import table.
      {
        //
        // Get number items.
        //

        a = pStream[p++], b = pStream[p++] << 8;
        int nItem = a | b;

        //
        // Read import items.
        //

        for (int i = 0; i < nItem;++i) {

          //
          // Read item data.
          //

          VMImpl::Import ip;
          ip.iMod = -1;                 // Later link.
          ip.offset = -1;
          ip.native = 0;
          a = p++, b = p++;
          ip.iModName = pStream[a] | (pStream[b] << 8);
          a = p++, b = p++;
          ip.iLabelName = pStream[a] | (pStream[b] << 8);

          //
          // Store the import item data.
          //

          mod.imports.push_back(ip);
        }
      }
      break;

    case TAG_CODE:                      // Code section.
      {
        //
        // Get code size.
        //

        a = pStream[p++], b = pStream[p++] << 8;
        int szCode = a | b;

        //
        // Read code data.
        //

        while (vm.hp + szCode > vm.heapSize) { // Need grow.
          unsigned char* heap = new unsigned char[2 * vm.heapSize];
          memcpy(heap, vm.heap, vm.hp);
          delete [] vm.heap;
          vm.heap = heap;
          vm.heapSize *= 2;
        }

        memcpy(vm.heap + vm.hp, pStream + p, szCode);
        mod.offset = vm.hp;
        mod.size = szCode;
        vm.hp += szCode;

        p += szCode;
      }
      break;

    case TAG_DONE:                      // End of mod tag.
      bDone = true;
      break;

    default:
      printf("Unknown section tag read '%s.m'\n", modName.c_str());
      return -1;

    }
  }

  mod.iMod = (int)vm.mod.size();
  mod.modName = modName;
  vm.mod.push_back(mod);

  return (int)(vm.mod.size() - 1);
}

// end of ld.cpp

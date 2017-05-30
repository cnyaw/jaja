//
// jaja tool, MOD(bin) to C array.
//
// 2009/08/14 Waync Cheng
//

#include <stdio.h>

#include <algorithm>
#include <string>

int main(int argc, char **argv)
{
  printf("jaja bin2c (alpha), 2009 Waync Cheng.\n");

  if (3 != argc) {
    printf("Uasage: bin2c ModName outName\n");
    return 1;
  }

  FILE* hin = fopen(argv[1], "rb");
  if (0 == hin) {
    printf("Open file '%s' failed\n", argv[1]);
    return 1;
  }

  FILE* hout = fopen(argv[2], "wt");
  if (0 == hout) {
    fclose(hin);
    printf("Open file '%s' failed\n", argv[2]);
    return 1;
  }

  std::string mod = argv[1];
  mod = mod.substr(0, mod.rfind(".", mod.size(), 1));
  std::string::size_type idx = mod.rfind("/", mod.size(), 1);
  if (0 != idx) {
    idx++;
  }
  mod = mod.substr(idx, mod.size());
  std::transform(mod.begin(), mod.end(), mod.begin(), toupper);

  fprintf(hout, "\n#ifndef %s_BIN\n#define %s_BIN\n", mod.c_str(), mod.c_str());
  fprintf(hout, "\nconst unsigned char %s_MOD[] = {\n  ", mod.c_str());

  int cnt = 0, c;
  while (EOF != (c = getc(hin))) {
    fprintf(hout, "0x%02x, ", c);
    cnt++;
    if (16 == cnt) {
      cnt = 0;
      fprintf(hout, "\n  ");
    }
  }

  fprintf(hout, "\n};\n");
  fprintf(hout, "\n#endif // %s_BIN\n\n// end of file %s\n", mod.c_str(), argv[2]);

  fclose(hin);
  fclose(hout);

  return 0;
}

// end of bin2c.cpp

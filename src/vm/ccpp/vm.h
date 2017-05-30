//
// jaja, vm instance.
//
// 2009/08/14 Waync Cheng
//

#ifndef JAJA_VM
#define JAJA_VM

#include <string>

class jajaVM
{
public:

  ~jajaVM();

  // interface
  static jajaVM& getInstance();

  bool invoke(std::string const& modName, std::string const& path = "");
  bool runStep();

protected:

  jajaVM();
};

#endif // JAJA_VM

// end of vm.h

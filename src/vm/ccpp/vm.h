//
// jaja, vm instance.
//
// 2009/08/14 Waync Cheng
//

#pragma once

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

// end of vm.h


#ifndef __CPPM__CMAKE_CMAKE_OPTION_H__
#define __CPPM__CMAKE_CMAKE_OPTION_H__

#include"cppm.h"
#include<string>

namespace cmake::option 
{
    std::string builder(cppm::Project& project);
    std::string compiler(cppm::Project& project);
    std::string build(cppm::Project& project);
    std::string luncher(cppm::Project& project);
}

#endif  
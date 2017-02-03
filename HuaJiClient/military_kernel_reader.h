

#ifndef _XGHJ_MILITARY_KERNEL_READER_
#define _XGHJ_MILITARY_KERNEL_READER_

#include <vector>
#include <string>
#include <fstream>

namespace XGHJ {

bool loadMilitaryKernel(std::vector<std::vector<float> >& militaryKernel, std::string mknl_filename);

}

#endif
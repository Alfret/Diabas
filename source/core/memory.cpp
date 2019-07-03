#include "core/memory.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <fstream>

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::core {

void
GetVirtualMemoryUsage(f64& vmUsage, f64& residentSet)
{
#if defined(_WIN32)
  return 0;
#else
  using std::ifstream;
  using std::ios_base;
  using std::string;

  vmUsage = 0.0;
  residentSet = 0.0;

  ifstream stat_stream("/proc/self/stat", ios_base::in);

  // dummy vars for leading entries in stat that we don't care about
  //
  string pid, comm, state, ppid, pgrp, session, tty_nr;
  string tpgid, flags, minflt, cminflt, majflt, cmajflt;
  string utime, stime, cutime, cstime, priority, nice;
  string O, itrealvalue, starttime;

  // the two fields we want
  //
  unsigned long vsize;
  long rss;

  stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >>
    tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >>
    stime >> cutime >> cstime >> priority >> nice >> O >> itrealvalue >>
    starttime >> vsize >> rss; // don't care about the rest

  stat_stream.close();

  long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024;
  vmUsage = vsize / 1024.0;
  residentSet = rss * page_size_kb;
#endif
}

}
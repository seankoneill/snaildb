#ifndef SNAILDB_LIBS_LSM_TREE_RECORD_H_
#define SNAILDB_LIBS_LSM_TREE_RECORD_H_

#include <cstring>
#include <ostream>
#include <istream>

namespace snaildb {

struct Record {
  std::string key;
  std::string value;

  std::ostream& operator<<(std::ostream& os) {
    os << key;
    os << value;
    return os;
  }

  std::istream& operator>>(std::istream& is) {
    is >> key;
    is >> value;
    return is;
  }
};

} //namespace snaildb

#endif //SNAILDB_LIBS_LSM_TREE_RECORD_H_

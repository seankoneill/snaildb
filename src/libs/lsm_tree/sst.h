#ifndef SNAILDB_LIBS_LSM_TREE_SST_H_
#define SNAILDB_LIBS_LSM_TREE_SST_H_

#include <string>
#include <fstream>
#include <vector>
#include <optional>
#include <map>
#include <iostream>

#include "segment.h"

namespace snaildb {

  //sorted string table
  class SST {
    public:
      ~SST();

      bool open(std::string dbName);
      bool compact();

      bool write(std::map<std::string, std::string> mem_table);
      std::optional<std::string> get(std::string key);

    private:
      //try adding keys to sparse index randomly for now
      std::fstream table_file_;
      std::vector<Segment*> segments_;

  };

} //namespace snaildb

#endif //SNAILDB_LIBS_LSM_TREE_SST_H_

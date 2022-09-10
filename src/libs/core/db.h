#ifndef SNAILDB_SRC_LIBS_CORE_DB_H_
#define SNAILDB_SRC_LIBS_CORE_DB_H_

#include <stdlib.h>
#include <string.h>
#include <optional>
#include <unordered_map>

#include "lsm.h"

namespace snaildb {

  class DB {
    public:
      std::optional<std::string> get(std::string key);
      bool put(std::string key, std::string value);
      bool remove(std::string key);

    private:
      LsmTree storage_engine_;
      
  };

}; //namespace snaildb

#endif //SNAILDB_SRC_LIBS_CORE_DB_H_

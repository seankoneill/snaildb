#include "sst.h"

#include "spdlog/spdlog.h"
#include <iterator>

namespace snaildb {

  SST::~SST() {
    table_file_.close();
  }

  bool SST::open(std::string dbName) {
    table_file_.open(dbName, std::ios::in | std::ios::app | std::ios::binary);
    if (!table_file_.good()) {
      spdlog::error("failed to open SST");
      return false;
    }
    return true;
  }

  bool SST::write(std::map<std::string, std::string> mem_table) {
    Segment* s = new Segment(table_file_);
    segments_.push_back(s);
    s->write(mem_table);
    return true;
  }

  bool SST::compact() {
    for (auto& s: segments_) {
    }
    return true;
  }

  std::optional<std::string> SST::get(std::string key) {
    spdlog::debug("searching {} segments_ for: {}",segments_.size(),key);
    for(auto&& e = segments_.rbegin(); e != segments_.rend(); ++e) {
      int i = 1;
      std::optional<std::string> val = (*e)->get(key);
      if (val.has_value()) {
        spdlog::debug("found {} in segment no. {} (starts from back)",key,i);
        return val.value();
      }
      ++i;
    }
    return {"sst get: not found"};
  }
}

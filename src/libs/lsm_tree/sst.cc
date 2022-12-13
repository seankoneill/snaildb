#include "sst.h"

#include "spdlog/spdlog.h"

#include <iterator>
#include <stdexcept>

#include <filesystem>

namespace snaildb {

SST::~SST() {
  table_file_.close();
}

void SST::close() {
  table_file_.close();
}

void SST::readFromFile() {
  segments_.clear();
  size_t read_offset = 0;
  while ((!table_file_.eof()) && table_file_.good()) {
    segments_.push_back(std::make_unique<Segment>(table_file_,read_offset));
  }
  table_file_.clear();
}

void SST::open(std::string file_path) {
  table_file_.open(file_path, std::ios::in | std::ios::out | std::ios::app);
  if (!table_file_.good()) {
    throw std::runtime_error(fmt::format("Couldn't open db file {}", file_path));
    return;
  }

  if (!std::filesystem::is_empty(file_path)) {
    readFromFile(); 
  }

  this->db_name_ = file_path;
}

void SST::write(std::map<std::string, std::string> mem_table) {
  segments_.push_back(std::make_unique<Segment>(table_file_,mem_table));
}

void SST::compact() {
  for (auto& s: segments_) {
  }
}

std::optional<std::string> SST::get(std::string key) const {
  spdlog::debug("searching {} segments_ for: {}",segments_.size(),key);

  for(auto&& e = segments_.rbegin(); e != segments_.rend(); ++e) {
    int i = 1;
    std::optional<std::string> val = e->get()->findKey(key);
    if (val.has_value()) {
      spdlog::debug("found {} in segment no. {} (starts from back)",key,++i);
      return val.value();
    }
  }
  
  return {};
}

} //namespace snaildb

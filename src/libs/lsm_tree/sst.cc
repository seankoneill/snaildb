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
  table_file_.seekg(std::ios::end);
  size_t end_offset = table_file_.tellg();

  while ((!table_file_.eof()) && table_file_.good()) {
    std::unique_ptr<Segment> s = std::make_unique<Segment>();
    //s->readFromFile();
    segments_.push_back(std::move(s));
  }

  table_file_.clear();
}

void SST::open(std::string file_path) {
  table_file_.open(file_path, std::ios::in | std::ios::out | std::ios::app);
  if (!table_file_.good()) {
    throw std::runtime_error(fmt::format("Couldn't open db file. State: {}", table_file_.rdstate()));
    return;
  }

  if (!std::filesystem::is_empty(file_path)) {
    readFromFile(); 
  }

  this->db_name_ = file_path;
}

void SST::write(std::map<std::string, std::string> mem_table) {
  //std::unique_ptr<Segment> s = std::make_unique<Segment>(table_file_);
  //s->writeTo(mem_table);
  //segments_.push_back(std::move(s));
}

void SST::compact() {
  for (auto& s: segments_) {
  }
}

std::optional<std::string> SST::get(std::string key) const {
  spdlog::debug("searching {} segments_ for: {}",segments_.size(),key);

  int i = 1;
  for(auto e = segments_.rbegin(); e != segments_.rend(); ++e) {

    spdlog::debug("Searching segment {} for: {}",i,key);

    std::optional<std::string> val = e->get()->findKey(key);
    if (val.has_value()) {
      spdlog::debug("found {} in segment no. {}.",key,i);
      return val.value();
    }
    ++i;
  }
  
  return {};
}

} //namespace snaildb

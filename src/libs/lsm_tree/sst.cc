#include "sst.h"

#include "spdlog/spdlog.h"

#include <iterator>
#include <stdexcept>

#include <filesystem>

namespace snaildb {

/*
Reads a SST in from collection of segment files 
in a directory. Currently assumes that all files in 
the directory ending in .seg are segment files.
*/
void SST::open(std::filesystem::path dir_path) {
  if (std::filesystem::exists(dir_path) && !std::filesystem::is_directory(dir_path)) {
    throw std::runtime_error("Path for SST must be a directory or not already exist.");
  }

  if (!std::filesystem::exists(dir_path)) {
    std::filesystem::create_directory(dir_path);
  }

  db_directory_ = dir_path;

  for (std::filesystem::directory_entry de : std::filesystem::directory_iterator(dir_path)) {
    if (de.path().extension().string() != ".seg") {
      continue;
    }
    std::unique_ptr<Segment> s = std::make_unique<Segment>();
    s->readFromFile(de);
    segments_.push_back(std::move(s));
  }

  next_id_ = segments_.size() + 1;
}

std::filesystem::path SST::nextSegmentPath() {
  return db_directory_.string() + std::to_string(next_id_++) + ".seg";
}

void SST::write(std::map<std::string, std::string> mem_table) {
  std::unique_ptr<Segment> s = std::make_unique<Segment>();
  s->writeTo(nextSegmentPath(),mem_table);
  segments_.push_back(std::move(s));
}

void SST::compact(std::vector<Segment>& segments) {
  std::map<std::string, std::string> new_table;
  for (auto& s: segments) {
  }
}

std::optional<std::string> SST::get(std::string key) const {
  spdlog::debug("searching {} segments_ for: {}",segments_.size(),key);

  int i = segments_.size();
  for(auto e = segments_.rbegin(); e != segments_.rend(); ++e) {

    spdlog::debug("Searching segment {} for: {}",i,key);

    std::optional<std::string> val = e->get()->findKey(key);
    if (val.has_value()) {
      spdlog::debug("found {} in segment no. {}.",key,i);
      return val.value();
    }
    --i;
  }
  
  return {};
}

} //namespace snaildb

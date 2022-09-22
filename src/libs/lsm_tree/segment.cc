#include <filesystem>

#include "segment.h"

#include "spdlog/spdlog.h"

namespace snaildb {

Segment::Segment(std::fstream& tf) : 
  table_file_(tf),
  filter_(new CountingBloomFilter<uint16_t>(100,100)) 
{}

Segment::~Segment() {
  delete filter_;
}

//write a whole table to a new segment
bool Segment::write(std::map<std::string,std::string> mem_table) {
  //start a transaction?

  if (table_file_.bad()) {
    spdlog::error("Something went wrong in Segment::write");
    return false;
  }

  size_t bytes_written = 0; //bytes written since last offset was added to the index

  spdlog::debug("Writing new segment starting at byte: {} in table file", table_file_.tellp());

  for (auto& e : mem_table) {

    spdlog::debug("Adding entry to table file {}:{} at byte {}", e.first,e.second,table_file_.tellp());

    if (bytes_written == 0 || e == *mem_table.begin() || e == *(--mem_table.end())) {
      sparse_index_[e.first] = table_file_.tellp();
    }

    char keySize = e.first.size();
    char valueSize = e.second.size();
    const char * key = e.first.c_str();
    const char * value = e.second.c_str();

    table_file_.put(keySize);
    table_file_.put(valueSize);
    table_file_.write(key,keySize);
    table_file_.write(value,valueSize);

    bytes_written += 2 + e.first.size() + e.second.size();

    filter_->add(e.first.c_str(),e.first.size());

    if (bytes_written > INDEX_THRESHHOLD_) {
      bytes_written = 0;
    }
  }

  return true;
}

std::optional<std::string> Segment::get(std::string key) {
  if (!filter_->contains(key.c_str(),key.size())) {
    return {};
  }

  auto next_record = sparse_index_.upper_bound(key); //greater or equal record
  size_t starting_offset;
  size_t upper_bound;

  if (next_record == sparse_index_.end()) {
    upper_bound = (--sparse_index_.end())->second;
  } else {
    upper_bound = next_record->second;
  }

  if (next_record == sparse_index_.begin()) {
    return {};
  } else {
    starting_offset = (--next_record)->second;
  }

  spdlog::debug("Searching for {} in segment {}:{}",key,starting_offset,upper_bound);

  while (starting_offset <= upper_bound) {
    std::pair<std::string, std::string> pair = readRecord(starting_offset);
    spdlog::debug("pair at byte {}, {}:{}",starting_offset,pair.first,pair.second);
    if (pair.first == key) {
      return { pair.second };
    }
  }

  return {};
}

std::string Segment::findKey(std::string, size_t offset) {
  return {};
}

std::pair<std::string, std::string> Segment::readRecord(size_t& offset) {
  if (table_file_.bad()) {
    spdlog::error("Something went wrong in Segment::readRecord: {0:b}", table_file_.rdstate());
    exit(1);
  }
  spdlog::debug("Reading record starting at offest: {}",offset);
  table_file_.seekg(offset);

  char keySize;
  char valueSize;

  table_file_.get(keySize);
  table_file_.get(valueSize);

  char key[keySize];
  char value[valueSize];
  key[sizeof key] = '\0';
  value[sizeof value] = '\0';

  table_file_.read(key, keySize);
  table_file_.read(value, valueSize);

  offset = table_file_.tellg();

  return {key,value};
}
}

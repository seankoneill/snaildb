#include <filesystem>

#include "segment.h"

#include "spdlog/spdlog.h"

namespace snaildb {

Segment::Segment(std::fstream& tf) : 
  table_file_(tf) {}

Segment::Segment(std::fstream& tf, std::map<std::string,std::string>& mt) : 
  Segment(tf) 
{
  write(mt);
}

Segment::Segment(std::fstream& tf, size_t offset):
  Segment(tf) 
{
  read(offset);
}

void Segment::read(size_t offset) {
  if (table_file_.bad()) {
    spdlog::debug("Segment::read failed at file offset: {}",offset);
    throw std::runtime_error("Failed to read segment. Bad table file");
  }

  table_file_.seekg(offset);

  std::string magic_bytes;
  table_file_ >> magic_bytes;

  if (magic_bytes != "SS") {
    spdlog::debug("Segment::read failed at file offset: {}",offset);
    throw std::runtime_error("Failed to read segment. No segment beginning at offset.");
  }

  size_t bytes_read = 3; //bytes read since last offset was added to the index
  size_t total_bytes_read = 3; 
  std::string key;
  std::string value;

  while (!table_file_.eof() && key != "SS") {
    table_file_ >> key;
    table_file_ >> value;

    filter_.add(key.c_str(),key.size());

    if (bytes_read > INDEX_THRESHHOLD_) {
      sparse_index_[key] = bytes_read;
      bytes_read = 0;
    }

    bytes_read +=  key.size() + value.size();
    total_bytes_read += bytes_read;
  }
}

//write a whole table to a new segment
void Segment::write(std::map<std::string,std::string>& mem_table) {
  if (table_file_.bad()) {
    throw std::invalid_argument("Failed to write segment. Bad table file");
  }

  table_file_.seekp(std::ios::beg);

  spdlog::debug("Writing new segment starting at byte: {} in table file", table_file_.tellp());
  table_file_ << "SS" << "\n";

  size_t bytes_written = 3; //bytes written since last offset was added to the index
  size_t total_bytes_written = 3; 
  for (auto& e : mem_table) {

    spdlog::debug("Adding entry to table file {}:{} at byte {}", e.first,e.second,table_file_.tellp());

    if (bytes_written == 0 || e == *mem_table.begin() || e == *(--mem_table.end())) {
      sparse_index_[e.first] = table_file_.tellp();
    }

    table_file_ << e.first << "\n";
    table_file_ << e.second << "\n";

    bytes_written += 2 + e.first.size() + e.second.size();

    filter_.add(e.first.c_str(),e.first.size());

    if (bytes_written > INDEX_THRESHHOLD_) {
      bytes_written = 0;
    }
  }
}

std::optional<std::string> Segment::findKey(std::string key) const {
  if (!filter_.contains(key.c_str(),key.size())) {
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

  while (starting_offset <= upper_bound && !table_file_.eof()) {
    std::pair<std::string,std::string> r = readRecord(starting_offset);
    spdlog::debug("pair at byte {}, {}:{}",starting_offset,r.first,r.second);
    if (r.first == key) {
      return { r.second };
    }
    starting_offset += r.first.size() + r.second.size() + 2;
  }

  return {};
}

std::pair<std::string,std::string> Segment::readRecord(size_t offset) const {
  if (table_file_.bad()) {
    spdlog::error("Something went wrong in Segment::readRecord: {0:b}", table_file_.rdstate());
    throw std::runtime_error("Bad table file");
  }

  spdlog::debug("Reading record starting at offset: {}",offset);
  table_file_.seekg(offset);

  std::string key;
  std::string value;

  table_file_ >> key;
  table_file_ >> value;

  return {key,value};
}

} //namespace snaildb

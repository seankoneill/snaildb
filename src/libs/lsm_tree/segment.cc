#include <filesystem>

#include "segment.h"

#include "spdlog/spdlog.h"

namespace snaildb {

Segment::Segment(std::fstream& tf) : 
  table_file_(tf) {}

void Segment::readFromFile(size_t& offset) {
  if (!table_file_.good()) {
    throw std::runtime_error("Failed to read segment. Bad table file");
  }

  table_file_.seekg(offset);
  if (!table_file_.good()) {
    throw std::runtime_error("Failed to read segment. Bad file offset.");
  }

  std::string magic_bytes;
  table_file_ >> magic_bytes;

  if (magic_bytes != "SS") {
    throw std::runtime_error("Failed to read segment. No segment beginning at offset.");
  }

  size_t bytes_read = 3; //bytes read since last offset was added to the index
  size_t total_bytes_read = 3; 
  this->start_offset_ = offset;
  std::string key;
  std::string value;

  while (true) {

    table_file_ >> key;
    table_file_ >> value;
    if (table_file_.eof() || key == "SS") break;

    filter_.add(key.c_str(),key.size());

    if (bytes_read > INDEX_THRESHHOLD_ || total_bytes_read < INDEX_THRESHHOLD_) {
      sparse_index_[key] = offset + total_bytes_read;
      bytes_read = 0;
    }

    bytes_read += 2 + key.size() + value.size();
    total_bytes_read += 2 + key.size() + value.size();
  }
  this->end_offset_ = offset + total_bytes_read;
  offset = end_offset_;
}

//write a whole table to a new segment
void Segment::write(std::map<std::string,std::string>& mem_table) {
  if (table_file_.bad()) {
    throw std::invalid_argument("Failed to write segment. Bad table file");
  }

  table_file_.seekp(0,std::ios_base::end);

  spdlog::debug("Writing new segment starting at byte: {} in table file.", table_file_.tellp());

  table_file_ << "SS\n";
  start_offset_ = table_file_.tellp();

  size_t bytes_written = 0; //bytes written since last offset was added to the index
  size_t total_bytes_written = 0; 
  for (auto e : mem_table) {
    if (bytes_written == 0) {
      sparse_index_[e.first] = total_bytes_written;
    }

    spdlog::debug("Writing pair: {},{} in table file.", e.first,e.second);
    table_file_ << e.first << "\n";
    table_file_ << e.second << "\n";

    bytes_written += 2 + e.first.size() + e.second.size();
    total_bytes_written += 2 + e.first.size() + e.second.size();

    filter_.add(e.first.c_str(),e.first.size());

    if (bytes_written > INDEX_THRESHHOLD_) {
      bytes_written = 0;
    }
  }
  end_offset_ = start_offset_ + total_bytes_written;
  spdlog::debug("Wrote new segment starting at bytes: {},{} in table file.", start_offset_, end_offset_);
}

std::optional<std::string> Segment::findKey(std::string key) const {
  if (!filter_.contains(key.c_str(),key.size())) {
    return {};
  }

  auto next_record = sparse_index_.upper_bound(key); //greater or equal record
  size_t starting_offset;
  size_t upper_bound;

  if (next_record == sparse_index_.end()) {
    upper_bound = this->end_offset_;
  } else {
    upper_bound = next_record->second;
  }

  if (next_record == sparse_index_.begin()) {
    starting_offset = this->start_offset_;
  } else {
    starting_offset = (--next_record)->second;
  }

  spdlog::debug("Searching for {} between bytes: {},{}",key,starting_offset,upper_bound);

  while (starting_offset <= upper_bound && !table_file_.eof()) {
    std::pair<std::string,std::string> r = readRecord(starting_offset);
    spdlog::debug("Read pair {}:{} at offset {}",r.first,r.second,starting_offset);
    if (r.first == key) {
      return { r.second };
    }
    starting_offset += r.first.size() + r.second.size() + 2;
  }

  return {};
}

std::pair<std::string,std::string> Segment::readRecord(size_t offset) const {
  if (!table_file_.good()) {
    spdlog::error("Something went wrong in Segment::readRecord. File state: {0:b}", table_file_.rdstate());
    throw std::runtime_error("Bad table file.");
  }

  table_file_.seekg(offset);

  std::string key;
  std::string value;

  table_file_ >> key;
  table_file_ >> value;

  return {key,value};
}

} //namespace snaildb

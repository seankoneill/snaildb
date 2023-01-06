#include <filesystem>

#include "segment.h"

#include "spdlog/spdlog.h"

namespace snaildb {

Segment::Segment(size_t index_threshold, size_t hashes, size_t width): 
  filter_(hashes,width),
  INDEX_THRESHHOLD_(index_threshold) {}

void Segment::readFromFile(std::filesystem::path p) {

  if (table_file_.is_open()) table_file_.close();

  table_file_.open(p,std::ios::binary | std::ios::in);

  if (!table_file_.good()) {
    throw std::runtime_error("Failed to read segment. Bad table file");
  }

  size_t bytes_read = 0;

  while (!table_file_.eof()) {

    size_t read_pos = table_file_.tellg();

    std::pair<std::string,std::string> r = nextRecord();
    bytes_read += r.first.size() + r.second.size() + 2;

    if (bytes_read > INDEX_THRESHHOLD_) {
      sparse_index_[r.first] = read_pos;
      bytes_read = 0;
    }
  }
}

//write a whole table to a new segment
void Segment::writeTo(std::filesystem::path p, std::map<std::string,std::string>& mem_table) {
  table_file_.open(p,std::ios::binary | std::ios::out| std::ios::in | std::ios::app);

  if (table_file_.bad()) {
    throw std::invalid_argument("Failed to write segment. Bad table file");
  }

  size_t bytes_written = 0; //bytes written since last offset was added to the index
  for (auto e : mem_table) {
    size_t write_pos = table_file_.tellp();

    uint8_t ks = e.first.size();
    uint8_t vs = e.second.size();
    table_file_.write(reinterpret_cast<char*>(&ks),1);
    table_file_.write(reinterpret_cast<char*>(&vs),1);
    table_file_.write(e.first.c_str(),e.first.size());
    table_file_.write(e.second.c_str(),e.second.size());

    bytes_written += 2 + e.first.size() + e.second.size();

    filter_.add(e.first.c_str(),e.first.size());

    if (bytes_written > INDEX_THRESHHOLD_) {
      sparse_index_[e.first] = write_pos;
      bytes_written = 0;
    }
  }
}

std::optional<std::string> Segment::findKey(std::string key) {
  if (!filter_.contains(key.c_str(),key.size())) {
    return {};
  }

  auto next_record = sparse_index_.upper_bound(key); //greater or equal record
  size_t starting_offset;
  size_t upper_bound;

  if (next_record == sparse_index_.end()) {
    upper_bound = SIZE_MAX;
  } else {
    upper_bound = next_record->second;
  }

  if (next_record == sparse_index_.begin()) {
    starting_offset = table_file_.beg;
  } else {
    starting_offset = (--next_record)->second;
  }

  spdlog::debug("Searching for {} between bytes: {},{}",key,starting_offset,upper_bound);

  table_file_.clear();
  table_file_.seekg(starting_offset);
  while (table_file_.tellg() <= upper_bound && !table_file_.eof()) {

    std::pair<std::string,std::string> r = nextRecord();

    if (r.first == key) {
      return { r.second };
    }
  }

  spdlog::debug("Filter false positive or error has occurred");
  return {};
}

std::pair<std::string,std::string> Segment::nextRecord() {
  if (!table_file_.good()) {
    spdlog::error("Something went wrong in Segment::nextRecord. File state: {0:b}", table_file_.rdstate());
    throw std::runtime_error("Bad table file.");
  }

  uint8_t key_size;
  uint8_t val_size;

  table_file_.read(reinterpret_cast<char*>(&key_size),1);
  table_file_.read(reinterpret_cast<char*>(&val_size),1);

  char key_buf[key_size+1];
  key_buf[key_size] = '\0';
  char val_buf[val_size+1];
  val_buf[val_size] = '\0';

  table_file_.read(key_buf,key_size);
  table_file_.read(val_buf,val_size);

  std::string key = std::string(key_buf);
  std::string val = std::string(val_buf);
  
  return {key,val};
}

} //namespace snaildb

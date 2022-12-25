#include <map>
#include <spdlog/spdlog.h>
#include <vector>
#include <fstream>

#include <catch2/catch_test_macros.hpp>

#include "../libs/core/db.h"
#include "test_util.h"

TEST_CASE("Segment write full index") {
  spdlog::set_level(spdlog::level::debug);
  std::string file_name = "segTestDB";
  std::string file_path = std::filesystem::current_path().string();
  std::string full_path = file_path + "/" + file_name + ".db";

  std::filesystem::remove(full_path);
  std::fstream f;
  f.open(full_path, std::ios::in | std::ios::out | std::ios::app);

  snaildb::Segment s(f);
  s.setIndexThreshold(1);

  std::map<std::string, std::string> test_map;
  std::vector<std::string> test_keys;
  std::vector<std::string> test_vals;

  size_t bytes_written = 0;
  for (int i = 0; i < 20; ++i) {
    test_keys.push_back(snaildb::TestUtil::random_string(5));
    test_vals.push_back(snaildb::TestUtil::random_string(10));

    bytes_written += test_keys[i].size() + test_vals[i].size() + 2;

    test_map[test_keys[i]] = test_vals[i];
  }

  s.write(test_map);

  REQUIRE(s.getStartOffset() == 0);
  REQUIRE(s.getEndOffset() == 3 + bytes_written);

  for (auto e: test_keys) {
    REQUIRE(s.findKey(e).value() == test_map[e]);
  }
}

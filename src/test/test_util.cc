#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace snaildb {

namespace TestUtil {

std::string random_string( size_t length )
{
  auto randchar = []() -> char
    {
      const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
      const size_t max_index = (sizeof(charset) - 1);
      std::random_device rd;  //Will be used to obtain a seed for the random number engine
      std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
      std::uniform_int_distribution<> distrib(0, max_index - 1);
      //int r = rand();                      // SOMEHOW KEPT GIVING SAME SEQUENCE????
      //std::cout << r << std::endl;
      //return charset[ r % max_index ];
      return charset[ distrib(gen) ];
    };
  std::string str(length,0);
  std::generate_n( str.begin(), length, randchar );
  return str;
}

} // namespace TestUtil

} // namespace snaildb

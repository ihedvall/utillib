/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <boost/beast/core/detail/base64.hpp>
#include <gtest/gtest.h>

using namespace boost::beast::detail;

namespace util::test {

TEST(Boost_Base64, EncodeDecode) { // NOLINT

for (int buffer_size = 1; buffer_size < 100; ++buffer_size) {
  for (int byte = 0x0; byte <= 0xFF; ++byte) {
    try {
      const std::vector<uint8_t> source(buffer_size, static_cast<uint8_t>(byte));
      const auto dest_size = base64::encoded_size(source.size());
      // std::cout << "Size: " << source.size() << "->" << dest_size << std::endl;
      std::string dest(dest_size, '\0');
      base64::encode(dest.data(), source.data(), source.size());
      // std::cout << "Base64: " << dest << std::endl;

      const auto source1_size = base64::decoded_size(dest.size());
      std::vector<uint8_t> source1(source1_size, 0);
      const auto source1_pair = base64::decode(source1.data(), dest.data(), dest.size());
      EXPECT_EQ(source.size(), source1_pair.first);
      if (source1.size() > source1_pair.first) {
        source1.resize(source1_pair.first);
      }
      EXPECT_EQ(source, source1);
      // std::cout << dest << std::endl;
    } catch (const std::exception& err) {
      FAIL() << err.what();
    }
  }
}


}

} // end namespace
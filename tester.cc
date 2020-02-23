#include "tokenizer.h"

#include <vector>

#include <unicode/rbbi.h>

#include <gtest/gtest.h>

std::vector<UnicodeString> usVector(std::initializer_list<std::string> l) {
  std::vector<UnicodeString> us;
  std::for_each(l.begin(), l.end(),
		[&us](const std::string &word){us.push_back(UnicodeString::fromUTF8(word));});
  return us;
}

void compareSeq(const std::vector<UnicodeString> &expected,
		const std::vector<UnicodeString> &actual) {
  EXPECT_EQ(expected.size(), actual.size());
  for (int i = 0; i < expected.size(); i++) {
    EXPECT_EQ(expected[i], actual[i]);
  }
}

TEST(Tokenizer, NotNormalized) {
  sese::Tokenizer tokenizer;
  std::vector<UnicodeString> tokenized = tokenizer.tokenize("I love NY!");
  std::vector<UnicodeString> expected = usVector({"I", " ", "love", " ", "NY", "!"});
  compareSeq(expected, tokenized);
}

TEST(Tokenizer, Normalized) {
  sese::Tokenizer tokenizer;
  std::vector<UnicodeString> tokenized = tokenizer.tokenize("She loves NY guys!", true);
  std::vector<UnicodeString> expected = usVector({"she", "lov", "ny", "guy", "!"});
  compareSeq(expected, tokenized);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

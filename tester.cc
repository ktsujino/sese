#include "tokenizer.h"
#include "lexicon.h"

#include <vector>

#include <unicode/rbbi.h>

#include <gtest/gtest.h>

namespace sese {

std::vector<UnicodeString> usVector(std::initializer_list<std::string> l) {
  std::vector<UnicodeString> us;
  std::for_each(l.begin(), l.end(),
		[&us](const std::string &word){us.push_back(UnicodeString::fromUTF8(word));});
  return us;
}

template <class T>
void assertVectorEqual(const std::vector<T> &expected,
		       const std::vector<T> &actual) {
  EXPECT_EQ(expected.size(), actual.size());
  for (int i = 0; i < expected.size(); i++) {
    EXPECT_EQ(expected[i], actual[i]);
  }
}

TEST(Tokenizer, not_normalized) {
  sese::Tokenizer tokenizer;
  std::vector<UnicodeString> tokenized = tokenizer.tokenize("I love NY!");
  std::vector<UnicodeString> expected = usVector({"I", " ", "love", " ", "NY", "!"});
  assertVectorEqual(expected, tokenized);
}

TEST(Tokenizer, normalized) {
  sese::Tokenizer tokenizer;
  std::vector<UnicodeString> tokenized = tokenizer.tokenize("She loves NY guys!", true);
  std::vector<UnicodeString> expected = usVector({"she", "lov", "ny", "guy", "!"});
  assertVectorEqual(expected, tokenized);
}

TEST(Lexicon, conversion) {
  sese::Lexicon lexicon;
  const UnicodeString foo = UnicodeString::fromUTF8("foo");
  const UnicodeString bar = UnicodeString::fromUTF8("bar");
  const UnicodeString baz = UnicodeString::fromUTF8("baz");

  lexicon.setEntry(foo, 0);
  lexicon.setEntry(bar, 1);
  EXPECT_EQ(0, lexicon.token2id(foo));
  EXPECT_EQ(1, lexicon.token2id(bar));
  EXPECT_EQ(sese::Lexicon::kOutOfVocabularyId, lexicon.token2id(baz));
  EXPECT_EQ(foo, lexicon.id2token(0));
  EXPECT_EQ(bar, lexicon.id2token(1));
  EXPECT_EQ(sese::Lexicon::kOutOfVocabularyToken, lexicon.id2token(2));

  std::vector<UnicodeString> tokens({foo, bar, baz});
  std::vector<int> ids({0, 1, -1});
  std::vector<UnicodeString> revTokens({foo, bar, Lexicon::kOutOfVocabularyToken});

  assertVectorEqual(ids, lexicon.tokens2ids(tokens));
  assertVectorEqual(revTokens, lexicon.ids2tokens(ids));
}

} // namespace sese

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

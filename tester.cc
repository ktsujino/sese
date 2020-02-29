#include "tokenizer.h"
#include "lexicon.h"

#include <cstdio>
#include <fstream>
#include <vector>

#include <unicode/rbbi.h>
#include <unicode/ustream.h>

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
  Tokenizer tokenizer;
  std::vector<UnicodeString> tokenized = tokenizer.tokenize("I love NY!");
  std::vector<UnicodeString> expected = usVector({"I", " ", "love", " ", "NY", "!"});
  assertVectorEqual(expected, tokenized);
}

TEST(Tokenizer, normalized) {
  Tokenizer tokenizer;
  std::vector<UnicodeString> tokenized = tokenizer.tokenize("She loves NY guys!", true);
  std::vector<UnicodeString> expected = usVector({"she", "lov", "ny", "guy", "!"});
  assertVectorEqual(expected, tokenized);
}

TEST(Lexicon, conversion) {
  Lexicon lexicon;
  const UnicodeString foo = UnicodeString::fromUTF8("foo");
  const UnicodeString bar = UnicodeString::fromUTF8("bar");
  const UnicodeString baz = UnicodeString::fromUTF8("baz");

  lexicon.setEntry(foo, 0);
  lexicon.setEntry(bar, 1);
  EXPECT_EQ(0, lexicon.token2id(foo));
  EXPECT_EQ(1, lexicon.token2id(bar));
  EXPECT_EQ(Lexicon::outOfVocabularyId(), lexicon.token2id(baz));
  EXPECT_EQ(foo, lexicon.id2token(0));
  EXPECT_EQ(bar, lexicon.id2token(1));
  EXPECT_EQ(Lexicon::outOfVocabularyToken(), lexicon.id2token(2));

  std::vector<UnicodeString> tokens({foo, bar, baz});
  std::vector<WordID> expected_ids({0, 1, Lexicon::outOfVocabularyId()});
  std::vector<UnicodeString> expected_backtranslated_tokens({foo, bar, Lexicon::outOfVocabularyToken()});

  std::vector<WordID> actual_ids = lexicon.tokens2ids(tokens);
  assertVectorEqual(expected_ids, actual_ids);
  std::vector<UnicodeString> actual_backtranslated_tokens = lexicon.ids2tokens(actual_ids);
  assertVectorEqual(expected_backtranslated_tokens, actual_backtranslated_tokens);
}

TEST(Lexicon, save_and_load) {
  LexiconBuilder lexicon_builder;
  std::vector<UnicodeString> corpus = usVector({"foo", "bar", "foo"});
  lexicon_builder.readTokens(corpus);
  Lexicon generated_lexicon = lexicon_builder.getLexicon();

  std::string lexicon_file_name = "/tmp/test_lexicon_save_and_load.lexicon";
  std::ofstream ofs(lexicon_file_name);
  generated_lexicon.save(ofs);
  ofs.close();

  std::ifstream ifs(lexicon_file_name);
  Lexicon loaded_lexicon(ifs);
  
  std::vector<UnicodeString> query = usVector({"foo", "bar", "baz"});
  std::vector<WordID> expected_ids_lexicon({0, 1, Lexicon::outOfVocabularyId()});
  std::vector<UnicodeString> expected_backtranslated_tokens({query[0], query[1], Lexicon::outOfVocabularyToken()});

  std::vector<WordID> actual_ids_lexicon = loaded_lexicon.tokens2ids(query);
  assertVectorEqual(expected_ids_lexicon, actual_ids_lexicon);
  std::vector<UnicodeString> actual_backtranslated_tokens = loaded_lexicon.ids2tokens(actual_ids_lexicon);
  assertVectorEqual(expected_backtranslated_tokens, actual_backtranslated_tokens);
}
  
TEST(LexiconBuilder, read_and_get_lexicon) {
  LexiconBuilder lexicon_builder;
  std::vector<UnicodeString> corpus = usVector({"foo", "bar", "foo"});
  std::vector<WordID> expected_ids_builder({0, 1, 0});

  std::vector<WordID> actual_ids_builder = lexicon_builder.readTokens(corpus);
  assertVectorEqual(expected_ids_builder, actual_ids_builder);

  Lexicon lexicon = lexicon_builder.getLexicon();
  std::vector<UnicodeString> query = usVector({"foo", "bar", "baz"});
  std::vector<WordID> expected_ids_lexicon({0, 1, Lexicon::outOfVocabularyId()});
  std::vector<UnicodeString> expected_backtranslated_tokens({query[0], query[1], Lexicon::outOfVocabularyToken()});

  std::vector<WordID> actual_ids_lexicon = lexicon.tokens2ids(query);
  assertVectorEqual(expected_ids_lexicon, actual_ids_lexicon);
  std::vector<UnicodeString> actual_backtranslated_tokens = lexicon.ids2tokens(actual_ids_lexicon);
  assertVectorEqual(expected_backtranslated_tokens, actual_backtranslated_tokens);
}

} // namespace sese

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

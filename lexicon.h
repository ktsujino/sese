#ifndef _SESS_LEXICON_H_
#define _SESS_LEXICON_H_

#include <iostream>
#include <map>
#include <vector>

#include <gtest/gtest.h>
#include <unicode/unistr.h>

namespace sese {

// Converts word list into word ID list
class Lexicon {
public:
  Lexicon(std::istream &ist);
  void save(std::ostream &ost);
  std::vector<int> tokens2ids(const std::vector<UnicodeString> &tokens);
  std::vector<UnicodeString> ids2tokens(const std::vector<int> &ids);
  int token2id(const UnicodeString &token);
  UnicodeString id2token(const int id);

  static const int outOfVocabularyId();
  static const UnicodeString outOfVocabularyToken();

  friend class LexiconBuilder;
  FRIEND_TEST(Lexicon, conversion);
  FRIEND_TEST(Lexicon, save_and_load);

private:
  Lexicon(); // only to be used from friends
  void load(std::istream &ist);
  void setEntry(const UnicodeString &token, const int id);
  std::map<UnicodeString, int> token2id_;
  std::map<int, UnicodeString> id2token_;
};

// Constructs Lexicon from corpus
class LexiconBuilder {
public:
  std::vector<int> readTokens(const std::vector<UnicodeString> &tokens);
  Lexicon getLexicon();

private:
  Lexicon lexicon_;
};

} // namespace sese

# endif // #ifndef _SESS_LEXICON_H_

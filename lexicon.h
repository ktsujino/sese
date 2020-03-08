#ifndef _SESS_LEXICON_H_
#define _SESS_LEXICON_H_

#include <iostream>
#include <map>
#include <vector>

#include <gtest/gtest.h>
#include <unicode/unistr.h>

namespace sese {

using WordID = int; // for future adjustment of bit width

// Converts word list into word ID list
class Lexicon {
public:
  Lexicon(std::istream &ist);
  void save(std::ostream &ost) const;
  std::vector<WordID> tokens2ids(const std::vector<UnicodeString> &tokens) const;
  std::vector<UnicodeString> ids2tokens(const std::vector<WordID> &ids) const;
  WordID token2id(const UnicodeString &token) const;
  UnicodeString id2token(const WordID id) const;

  static const WordID outOfVocabularyId();
  static const UnicodeString outOfVocabularyToken();

private:
  Lexicon(); // only to be used from friends
  void load(std::istream &ist);
  void setEntry(const UnicodeString &token, const WordID id);
  std::map<UnicodeString, WordID> token2id_;
  std::map<WordID, UnicodeString> id2token_;

  friend class LexiconBuilder;
  FRIEND_TEST(Lexicon, conversion);
  FRIEND_TEST(Lexicon, save_and_load);
};

// Constructs Lexicon from corpus
class LexiconBuilder {
public:
  std::vector<WordID> readTokens(const std::vector<UnicodeString> &tokens);
  Lexicon &&getLexicon();

private:
  Lexicon lexicon_;
};

} // namespace sese

# endif // #ifndef _SESS_LEXICON_H_

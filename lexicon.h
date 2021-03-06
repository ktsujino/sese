#ifndef _SESE_LEXICON_H_
#define _SESE_LEXICON_H_

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
  std::vector<WordID> tokens2ids(const std::vector<icu::UnicodeString> &tokens) const;
  std::vector<icu::UnicodeString> ids2tokens(const std::vector<WordID> &ids) const;
  std::vector<int> ids2DocumentFrequencies(const std::vector<WordID> &ids) const;
  WordID token2id(const icu::UnicodeString &token) const;
  icu::UnicodeString id2token(const WordID &id) const;
  int id2DocumentFrequency(const WordID &id) const;
  int token2DocumentFrequency(const icu::UnicodeString &token) const;

  static const WordID outOfVocabularyId();
  static const icu::UnicodeString outOfVocabularyToken();

private:
  Lexicon(); // only to be used from friends
  void load(std::istream &ist);
  void setEntry(const icu::UnicodeString &token, const WordID id, const int frequency);
  std::map<icu::UnicodeString, WordID> token2id_;
  std::map<WordID, icu::UnicodeString> id2token_;
  std::map<WordID, int> document_frequency_;

  friend class LexiconBuilder;
  FRIEND_TEST(Lexicon, conversion);
  FRIEND_TEST(Lexicon, save_and_load);
};

// Constructs Lexicon from corpus
class LexiconBuilder {
public:
  std::vector<WordID> registerTokens(const std::vector<icu::UnicodeString> &tokens);
  Lexicon &&getLexicon();

private:
  Lexicon lexicon_;
};

} // namespace sese

# endif // #ifndef _SESE_LEXICON_H_

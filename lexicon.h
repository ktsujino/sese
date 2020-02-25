#ifndef _SESS_LEXICON_H_
#define _SESS_LEXICON_H_

#include <map>
#include <vector>

#include <gtest/gtest.h>
#include <unicode/unistr.h>

namespace sese {

// Converts word list into word ID list
class Lexicon {
public:
  Lexicon(const std::string &lexicon_file);
  void save(const std::string &lexicon_file);
  std::vector<int> tokens2ids(const std::vector<UnicodeString> &tokens);
  std::vector<UnicodeString> ids2tokens(const std::vector<int> &ids);
  int token2id(const UnicodeString &token);
  UnicodeString id2token(const int id);

  static const int kOutOfVocabularyId;
  static const UnicodeString kOutOfVocabularyToken;

  friend class LexiconBuilder;
  FRIEND_TEST(Lexicon, conversion);
  FRIEND_TEST(Lexicon, save_and_load);

private:
  Lexicon(); // only to be used from friends
  void load(const std::string &lexicon_file);
  void setEntry(const UnicodeString &token, const int id);
  std::map<UnicodeString, int> token2id_;
  std::map<int, UnicodeString> id2token_;
};

} // namespace sese

# endif // #ifndef _SESS_LEXICON_H_

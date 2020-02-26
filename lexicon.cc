#include "lexicon.h"

#include <fstream>
#include <map>
#include <vector>

#include <unicode/unistr.h>
#include <unicode/ustream.h>

namespace sese {

Lexicon::Lexicon(const std::string &lexicon_file) {
  load(lexicon_file);
}

void Lexicon::save(const std::string &lexicon_file) {
  std::ofstream ofs(lexicon_file);
  for (std::pair<UnicodeString, int> entry : token2id_) {
    ofs << entry.first << entry.second << std::endl;
  }
}

std::vector<int> Lexicon::tokens2ids(const std::vector<UnicodeString> &tokens) {
  std::vector<int> ids;
  for (const auto &token : tokens) {
    ids.push_back(token2id(token));
  }
  return ids;
}

std::vector<UnicodeString> Lexicon::ids2tokens(const std::vector<int> &ids) {
  std::vector<UnicodeString> tokens;
  for (const int id : ids) {
    tokens.push_back(id2token(id));
  }
  return tokens;
}

int Lexicon::token2id(const UnicodeString &token) {
  if (token2id_.count(token) > 0) {
    return token2id_[token];
  } else {
    return Lexicon::outOfVocabularyId();
  }
}

UnicodeString Lexicon::id2token(const int id) {
  if (id2token_.count(id) > 0) {
    return id2token_[id];
  } else {
    return Lexicon::outOfVocabularyToken();
  }
}

const int Lexicon::outOfVocabularyId() {
  static const int oovId = 0;
  return oovId;
}

const UnicodeString Lexicon::outOfVocabularyToken() {
  static const UnicodeString oovToken = UnicodeString::fromUTF8("__OOV__");
  return oovToken;
}

Lexicon::Lexicon() {
}

void Lexicon::setEntry(const UnicodeString &token, const int id) {
  token2id_[token] = id;
  id2token_[id] = token;
}

void Lexicon::load(const std::string &lexicon_file) {
  std::ifstream ifs(lexicon_file);
  UnicodeString token;
  int id;
  while (ifs >> token >> id) {
    setEntry(token, id);
  }
}


} // namespace sese

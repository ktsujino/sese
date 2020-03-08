#include "lexicon.h"

#include <fstream>
#include <map>
#include <vector>

#include <unicode/unistr.h>
#include <unicode/ustream.h>

namespace sese {

Lexicon::Lexicon(std::istream &ist) {
  load(ist);
}

void Lexicon::save(std::ostream &ost) const {
  for (const std::pair<UnicodeString, WordID> &entry : token2id_) {
    ost << entry.first << "\t" << entry.second << std::endl;
  }
}

std::vector<WordID> Lexicon::tokens2ids(const std::vector<UnicodeString> &tokens) const {
  std::vector<WordID> ids;
  for (const auto &token : tokens) {
    ids.push_back(token2id(token));
  }
  return ids;
}

std::vector<UnicodeString> Lexicon::ids2tokens(const std::vector<WordID> &ids) const {
  std::vector<UnicodeString> tokens;
  for (const WordID id : ids) {
    tokens.push_back(id2token(id));
  }
  return tokens;
}

WordID Lexicon::token2id(const UnicodeString &token) const {
  const auto &it = token2id_.find(token);
  if (it != token2id_.end()) {
    return it->second;
  } else {
    return Lexicon::outOfVocabularyId();
  }
}

UnicodeString Lexicon::id2token(const WordID id) const {
  const auto &it = id2token_.find(id);
  if (it != id2token_.end()) {
    return it->second;
  } else {
    return Lexicon::outOfVocabularyToken();
  }
}

const WordID Lexicon::outOfVocabularyId() {
  static const WordID oovId{-1};
  return oovId;
}

const UnicodeString Lexicon::outOfVocabularyToken() {
  static const UnicodeString oovToken = UnicodeString::fromUTF8("__OOV__");
  return oovToken;
}

Lexicon::Lexicon() {
}

void Lexicon::setEntry(const UnicodeString &token, const WordID id) {
  token2id_[token] = id;
  id2token_[id] = token;
}

void Lexicon::load(std::istream &ist) {
  UnicodeString token;
  WordID id;
  while (ist >> token >> id) {
    setEntry(token, id);
  }
}

std::vector<WordID> LexiconBuilder::readTokens(const std::vector<UnicodeString> &tokens) {
  std::vector<WordID> ids;
  for (const UnicodeString &token: tokens) {
    if (lexicon_.token2id_.count(token) ==  0) {
      WordID new_id = lexicon_.token2id_.size();
      lexicon_.token2id_[token] = new_id;
      lexicon_.id2token_[new_id] = token;
    }
    ids.push_back(lexicon_.token2id_[token]);
  }
  return ids;
}

Lexicon &&LexiconBuilder::getLexicon() {
  return std::move(lexicon_);
}

} // namespace sese

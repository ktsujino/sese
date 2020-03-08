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
    UnicodeString token = entry.first;
    WordID id = entry.second;
    int frequency = id2DocumentFrequency(id);
    ost << entry.first << "\t" << entry.second << "\t" << frequency << std::endl;
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

UnicodeString Lexicon::id2token(const WordID &id) const {
  const auto &it = id2token_.find(id);
  if (it != id2token_.end()) {
    return it->second;
  } else {
    return Lexicon::outOfVocabularyToken();
  }
}

int Lexicon::id2DocumentFrequency(const WordID &id) const {
  const auto &it = document_frequency_.find(id);
  if (it != document_frequency_.end()) {
    return it->second;
  } else {
    return 0;
  }
}

int Lexicon::token2DocumentFrequency(const UnicodeString &token) const {
  const auto &tit = token2id_.find(token);
  if (tit == token2id_.end()) {
    return 0;
  }
  const WordID id = tit->second;
  const auto &iit = document_frequency_.find(id);
  if (iit != document_frequency_.end()) {
    return iit->second;
  } else {
    return 0;
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

void Lexicon::setEntry(const UnicodeString &token, const WordID id, const int frequency) {
  token2id_[token] = id;
  id2token_[id] = token;
  document_frequency_[id] = frequency;
}

void Lexicon::load(std::istream &ist) {
  UnicodeString token;
  WordID id;
  int frequency;
  while (ist >> token >> id >> frequency) {
    setEntry(token, id, frequency);
  }
}

std::vector<WordID> LexiconBuilder::registerTokens(const std::vector<UnicodeString> &tokens) {
  std::vector<WordID> ids;
  for (const UnicodeString &token: tokens) {
    const auto &it = lexicon_.token2id_.find(token);
    if (it == lexicon_.token2id_.end()) {
      WordID new_id = lexicon_.token2id_.size();
      lexicon_.token2id_[token] = new_id;
      lexicon_.id2token_[new_id] = token;
    }
    ids.push_back(lexicon_.token2id_[token]);
  }
  std::set<WordID> unique_ids(ids.begin(), ids.end());
  for (const WordID &id: unique_ids) {
    lexicon_.document_frequency_[id] += 1;
  }
  return ids;
}

Lexicon &&LexiconBuilder::getLexicon() {
  return std::move(lexicon_);
}

} // namespace sese

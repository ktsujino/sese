#ifndef _SESS_INDEX_H
#define _SESS_INDEX_H

#include <iostream>
#include <vector>
#include <map>

#include <gtest/gtest.h>
#include <unicode/unistr.h>

#include "document.h"
#include "lexicon.h"
#include "tokenizer.h"

namespace sese {

class Index {
public:
  Index(std::istream &ist);
  void save(std::ostream &ost) const;
  std::vector<DocumentID> query(const std::vector<WordID> &keywords) const;

private:
  Index() {};
  void load(std::istream &ist);
  std::vector<DocumentID> wordID2DocumentList(const WordID &word_id) const;
  std::map<WordID, std::vector<DocumentID>> posting_lists_;

  friend class IndexBuilder;
};

class IndexBuilder {
public:
  IndexBuilder(const std::vector<Document> &documents, icu::Locale locale=icu::Locale::getRoot(), bool enable_normalize=true);
  Index &&getIndex();
  Lexicon &&getLexicon();

private:
  void addDocument(const Document &document, bool enable_normalize);
  std::vector<UnicodeString> tokenizeDocument(const Document &document, bool enable_normalize);
  Index index_;
  LexiconBuilder lexicon_builder_;
  Tokenizer tokenizer_;

  FRIEND_TEST(Index, query);
};

} // namespace sese

#endif // #ifndef _SESS_INDEX_H

#ifndef _SESE_INDEX_H
#define _SESE_INDEX_H

#include <iostream>
#include <vector>
#include <map>

#include <gtest/gtest.h>
#include <unicode/unistr.h>

#include "document.h"
#include "lexicon.h"
#include "query.h"
#include "tokenizer.h"

namespace sese {

struct MatchInfo {
  MatchInfo(DocumentID document_id,
	    int document_length,
	    std::vector<int> term_frequency):
    document_id(document_id),
    document_length(document_length),
    term_frequency(term_frequency){};
  DocumentID document_id;
  int document_length;
  std::vector<int> term_frequency;
};

class Index {
public:
  Index(std::istream &ist);
  void save(std::ostream &ost) const;
  std::vector<MatchInfo> query(const QueryInfo &query_info) const;

private:
  Index() {};
  void load(std::istream &ist);
  std::vector<DocumentID> calcMatchSet(const std::vector<WordID> &keywords) const;
  std::vector<DocumentID> wordID2DocumentList(const WordID &word_id) const;
  MatchInfo getMatchInfo(const DocumentID &document_id,
			 const QueryInfo &query_info) const;
  std::map<WordID, std::vector<DocumentID>> posting_lists_;
  std::map<DocumentID, int> document_length_;
  std::map<std::pair<DocumentID, WordID>, int> term_frequency_;

  friend class IndexBuilder;
};

class IndexBuilder {
public:
  IndexBuilder(const std::vector<Document> &documents, icu::Locale locale=icu::Locale::getRoot(), bool enable_normalize=true);
  Index &&getIndex();
  Lexicon &&getLexicon();

private:
  void addDocument(const Document &document, bool enable_normalize);
  std::vector<icu::UnicodeString> tokenizeDocument(const Document &document, bool enable_normalize);
  Index index_;
  LexiconBuilder lexicon_builder_;
  Tokenizer tokenizer_;

  FRIEND_TEST(Index, query);
};

} // namespace sese

#endif // #ifndef _SESE_INDEX_H

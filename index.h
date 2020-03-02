#ifndef _SESS_INDEX_H
#define _SESS_INDEX_H

#include <iostream>
#include <vector>
#include <map>

#include <gtest/gtest.h>
#include <unicode/unistr.h>

#include "lexicon.h" // for WordID
#include "document.h" // for DocumentID

namespace sese {

class Index {
public:
  Index(std::istream &ist);
  void save(std::ostream &ost);
  std::vector<DocumentID> query(const std::vector<WordID> &keywords);

private:
  Index();
  void load(std::istream &ist);
  std::vector<DocumentID> wordID2DocumentList(const WordID &word_id);
  std::map<WordID, std::vector<DocumentID>> posting_lists_;

  friend class IndexBuilder;
};

class IndexBuilder {
public:
  IndexBuilder();
  void addDocument(const DocumentID &document_id, const std::vector<WordID> &word_ids);  // true iff success
  Index getIndex();

private:
  DocumentID max_document_id_;
  Index index_;
};

} // namespace sese

#endif // #ifndef _SESS_INDEX_H

#include "index.h"

#include <cassert>
#include <iostream>
#include <vector>

#include <unicode/unistr.h>

#include "merge.h"

namespace sese {

Index::Index(std::istream &ist) {
  load(ist);
}

void Index::save(std::ostream &ost) {
  ost << posting_lists_.size() << std::endl;
  for (const std::pair<WordID, std::vector<DocumentID>> &entry : posting_lists_) {
    ost << entry.first << "\t" << entry.second.size() << "\t";
    for (int i = 0; i < entry.second.size(); i++) {
      if (i > 0) {
	ost << " ";
      }
      ost << entry.second[i];
    }
    ost << std::endl;
  }
}

std::vector<DocumentID> Index::query(const std::vector<WordID> &keywords) {
  std::vector<DocumentID> matched_doc_list;
  for (int i = 0; i < keywords.size(); i++) {
    std::vector<DocumentID> list = wordID2DocumentList(keywords[i]);
    if (i == 0) {
      matched_doc_list = list;
    } else {
      matched_doc_list = mergeSortedVectors(matched_doc_list, list);
    }
  }
  return matched_doc_list;
}

Index::Index() {
}

void Index::load(std::istream &ist) {
  int posting_lists_size;
  ist >> posting_lists_size;
  for (int word = 0; word < posting_lists_size; word++) {
    WordID word_id;
    int vector_size;
    std::vector<DocumentID> document_id_list;
    ist >> word_id >> vector_size;
    for (int doc = 0; doc < vector_size; doc++) {
      DocumentID doc_id;
      ist >> doc_id;
      document_id_list.push_back(doc_id);
    }
    posting_lists_[word_id] = document_id_list;
  }
}

std::vector<DocumentID> Index::wordID2DocumentList(const WordID &word_id) {
  if (posting_lists_.count(word_id) > 0) {
    return posting_lists_[word_id];
  } else {
    return std::vector<DocumentID>();
  }
}

IndexBuilder::IndexBuilder()
  : max_document_id_(kNonExistentDocID),
    index_() {
}

void IndexBuilder::addDocument(const DocumentID &document_id, const std::vector<WordID> &word_ids) {
  assert(document_id > max_document_id_);
  max_document_id_ = document_id;
  for (const auto &word_id : word_ids) {
    index_.posting_lists_[word_id].push_back(document_id);
  }
}

Index IndexBuilder::getIndex() {
  return index_;
}

} // namespace sese

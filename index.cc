#include "index.h"

#include <cassert>
#include <iostream>
#include <vector>

#include <gtest/gtest.h>
#include <unicode/unistr.h>

#include "document.h"
#include "lexicon.h"
#include "util.h"
#include "tokenizer.h"

namespace sese {

Index::Index(std::istream &ist) {
  load(ist);
}

void Index::save(std::ostream &ost) const {
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

std::vector<DocumentID> Index::query(const std::vector<WordID> &keywords) const {
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

std::vector<DocumentID> Index::wordID2DocumentList(const WordID &word_id) const {
  const auto &it = posting_lists_.find(word_id);
  if (it != posting_lists_.end()) {
    return it->second;
  } else {
    return std::vector<DocumentID>();
  }
}

IndexBuilder::IndexBuilder(const std::vector<Document> &documents, icu::Locale locale, bool enable_normalize) :
  tokenizer_(locale) {
  for (DocumentID document_id = 0; document_id < documents.size(); document_id++) {
    addDocument(documents[document_id], enable_normalize);
  }
}

Index IndexBuilder::getIndex() {
  return index_;
}

Lexicon IndexBuilder::getLexicon() {
  return lexicon_builder_.getLexicon();
}

void IndexBuilder::addDocument(const Document &document, bool enable_normalize) {
  std::vector<UnicodeString> tokens = tokenizeDocument(document, enable_normalize);
  std::vector<WordID> word_ids = lexicon_builder_.readTokens(tokens);
  for (const auto &word_id : word_ids) {
    index_.posting_lists_[word_id].push_back(document.document_id);
  }
  std::cout << "Indexed "
	    << document.title
	    << " with "
	    << tokens.size()
	    << " tokens"
	    << std::endl;
}

std::vector<UnicodeString> IndexBuilder::tokenizeDocument(const Document &document, bool enable_normalize) {
  std::set<UnicodeString> tokens;
  std::vector<UnicodeString> tokenized_title = tokenizer_.tokenize(document.title, enable_normalize);
  tokens.insert(tokenized_title.begin(), tokenized_title.end());
  for (const std::string &line : document.body) {
    std::vector<UnicodeString> tokenized_line = tokenizer_.tokenize(line, enable_normalize);
    tokens.insert(tokenized_line.begin(), tokenized_line.end());
  }
  return std::vector<UnicodeString>(tokens.begin(), tokens.end());
}

} // namespace sese

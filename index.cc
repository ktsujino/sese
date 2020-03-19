#include "index.h"

#include <cassert>
#include <iostream>
#include <vector>

#include <gtest/gtest.h>
#include <unicode/unistr.h>

#include "document.h"
#include "lexicon.h"
#include "tokenizer.h"
#include "util.h"

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
  ost << document_length_.size() << std::endl;
  for (const std::pair<DocumentID, int> &entry : document_length_) {
    ost << entry.first << "\t" << entry.second << std::endl;
  }
  ost << term_frequency_.size() << std::endl;
  for (const std::pair<std::pair<DocumentID, WordID>, int> &entry : term_frequency_) {
    ost << entry.first.first << "\t" << entry.first.second << "\t" << entry.second << std::endl;
  }
}

std::vector<MatchInfo> Index::query(const QueryInfo &query_info) const {
  std::vector<DocumentID> matched_doc_list = calcMatchSet(query_info.word_ids);
  std::vector<MatchInfo> match_info_list;
  for (const DocumentID &document_id : matched_doc_list) {
    match_info_list.push_back(getMatchInfo(document_id, query_info));
  }
  return match_info_list;
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
  int document_length_size;
  ist >> document_length_size;
  for (int doc = 0; doc < document_length_size; doc++) {
    DocumentID document_id;
    int document_length;
    ist >> document_id >> document_length;
    document_length_[document_id] = document_length;
  }
  int term_frequency_size;
  ist >> term_frequency_size;
  for (int tf = 0; tf < term_frequency_size; tf++) {
    DocumentID document_id;
    WordID word_id;
    int term_frequency;
    ist >> document_id >> word_id >> term_frequency;
    term_frequency_[std::make_pair(document_id, word_id)] = term_frequency;
  }
}

std::vector<DocumentID> Index::calcMatchSet(const std::vector<WordID> &keywords) const {
  std::vector<int> matched_doc_list;
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

std::vector<DocumentID> Index::wordID2DocumentList(const WordID &word_id) const {
  const auto &it = posting_lists_.find(word_id);
  if (it != posting_lists_.end()) {
    return it->second;
  } else {
    return std::vector<DocumentID>();
  }
}

MatchInfo Index::getMatchInfo(const DocumentID &document_id,
			      const QueryInfo &query_info) const {
  std::vector<int> term_frequency;
  for (const auto &word_id : query_info.word_ids) {
    const auto &it = term_frequency_.find(std::make_pair(document_id, word_id));
    if (it != term_frequency_.end()) {
      term_frequency.push_back(it->second);
    } else {
      term_frequency.push_back(0);
    }
  }
  return MatchInfo(document_id,
		   document_length_.find(document_id)->second,
		   term_frequency);
}

IndexBuilder::IndexBuilder(const std::vector<Document> &documents, icu::Locale locale, bool enable_normalize) :
  tokenizer_(locale) {
  for (DocumentID document_id = 0; document_id < documents.size(); document_id++) {
    addDocument(documents[document_id], enable_normalize);
  }
}

Index &&IndexBuilder::getIndex() {
  return std::move(index_);
}

Lexicon &&IndexBuilder::getLexicon() {
  return lexicon_builder_.getLexicon();
}

void IndexBuilder::addDocument(const Document &document, bool enable_normalize) {
  std::vector<icu::UnicodeString> tokens = tokenizeDocument(document, enable_normalize);
  std::vector<WordID> word_ids = lexicon_builder_.registerTokens(tokens);
  std::set<WordID> unique_word_ids(word_ids.begin(), word_ids.end());
  for (const auto &word_id : unique_word_ids) {
    index_.posting_lists_[word_id].push_back(document.document_id);
  }
  for (const auto &word_id : word_ids) {
    auto pair = std::make_pair(document.document_id, word_id);
    if (index_.term_frequency_.find(pair) == index_.term_frequency_.end()) {
      index_.term_frequency_[pair] = 0;
    }
    index_.term_frequency_[pair] += 1;
  }
  index_.document_length_[document.document_id] = word_ids.size();
}

std::vector<icu::UnicodeString> IndexBuilder::tokenizeDocument(const Document &document, bool enable_normalize) {
  std::vector<icu::UnicodeString> tokens;
  std::vector<icu::UnicodeString> tokenized_title = tokenizer_.tokenize(document.title, enable_normalize);
  tokens.insert(tokens.end(), tokenized_title.begin(), tokenized_title.end());
  for (const std::string &line : document.body) {
    std::vector<icu::UnicodeString> tokenized_line = tokenizer_.tokenize(line, enable_normalize);
    tokens.insert(tokens.end(), tokenized_line.begin(), tokenized_line.end());
  }
  return tokens;
}

} // namespace sese

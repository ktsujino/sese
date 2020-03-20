#ifndef _SESE_SEARCH_ENGINE_H_
#define _SESE_SEARCH_ENGINE_H_

#include <iostream>
#include <vector>

#include <unicode/unistr.h>
#include <unicode/rbbi.h>

#include "document.h"
#include "index.h"
#include "lexicon.h"
#include "query.h"
#include "ranker.h"

namespace sese {

class SearchEngine {
public:
  SearchEngine(std::istream &ist_lexicon,
	       std::istream &ist_index,
	       std::istream &ist_document_store,
	       icu::Locale locale=icu::Locale::getRoot());
  std::vector<Document> query(const std::string &keywords) const;

private:
  DocumentStore document_store_;
  Index index_;
  QueryProcessor query_processor_;
  Ranker ranker_;
};


} // namespace sese

# endif // #ifndef _SESE_SEARCH_ENGINE_H_



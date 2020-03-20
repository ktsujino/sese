#include "search_engine.h"

#include <fstream>
#include <iostream>
#include <vector>

#include <unicode/unistr.h>

#include "document.h"
#include "index.h"
#include "query.h"
#include "ranker.h"

namespace sese {

SearchEngine::SearchEngine(std::istream &ist,
			   icu::Locale locale) :
  document_store_(ist),
  index_(ist),
  query_processor_(ist)
{
}

std::vector<Document> SearchEngine::query(const std::string &keywords) const {
  const QueryInfo query_info = query_processor_.processQuery(keywords);
  const std::vector<MatchInfo> matched_documents = index_.query(query_info);
  const std::vector<std::pair<MatchInfo, RankScore>> ranked_results =
    ranker_.rank(matched_documents, query_info);
  std::vector<Document> documents;
  for (const std::pair<MatchInfo, RankScore> &item : ranked_results) {
    documents.push_back(document_store_.getDocument(item.first.document_id));
  }
  return documents;
}

} // namespace sese

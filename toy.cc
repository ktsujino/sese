#include <fstream>
#include <iostream>
#include <unicode/rbbi.h>
#include <unicode/ustream.h>

#include "document.h"
#include "index.h"
#include "lexicon.h"
#include "ranker.h"
#include "query.h"
#include "tokenizer.h"

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cout << "Usage: "
	      << argv[0]
	      << " <XML dump file> <query>"
	      << std::endl;
    return -1;
  }
  std::ifstream ifs(argv[1]);
  sese::DocumentReader document_reader;
  std::vector<sese::Document> documents = document_reader.readXML(ifs);
  sese::DocumentStore document_store = document_reader.getDocumentStore();
  sese::IndexBuilder index_builder;
  for (const sese::Document &document : documents) {
    index_builder.addDocument(document);
  }
  sese::Index index = index_builder.getIndex();
  sese::QueryProcessor query_processor(index_builder.getLexicon());
  sese::Ranker ranker;

  std::string keywords(argv[2]);
  sese::QueryInfo query_info = query_processor.processQuery(keywords);
  std::vector<sese::MatchInfo> results = index.query(query_info);
  std::vector<std::pair<sese::MatchInfo, sese::RankScore>> ranked_results = ranker.rank(results, query_info);
  std::cout << "Search results:" << std::endl;
  for (const std::pair<sese::MatchInfo, sese::RankScore> &result : ranked_results) {
    sese::Document document = document_store.getDocument(result.first.document_id);
    std::cout << document.title << "\t" << document.url << "\t" << result.second << std::endl;
  }
  return 0;
}

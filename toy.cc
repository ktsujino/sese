#include <fstream>
#include <iostream>
#include <unicode/rbbi.h>
#include <unicode/ustream.h>

#include "document.h"
#include "index.h"
#include "lexicon.h"
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
  sese::IndexBuilder index_builder(documents);
  sese::Index index = index_builder.getIndex();
  sese::QueryProcessor query_processor(index_builder.getLexicon());

  std::string keywords(argv[2]);
  sese::QueryInfo query_info = query_processor.processQuery(keywords);
  std::vector<sese::MatchInfo> results = index.query(query_info);
  std::cout << "Search results:" << std::endl;
  for (const sese::MatchInfo &result : results) {
    sese::Document document = document_store.getDocument(result.document_id);
    std::cout << document.title << "\t" << document.url << std::endl;
  }
  return 0;
}

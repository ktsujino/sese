#ifndef _SESE_QUERY_H_
#define _SESE_QUERY_H_

#include <iostream>
#include <vector>

#include <unicode/unistr.h>
#include <unicode/rbbi.h>

#include "lexicon.h"
#include "tokenizer.h"

namespace sese {

struct QueryInfo {
  QueryInfo(std::string keywords,
	    std::vector<icu::UnicodeString> tokens,
	    std::vector<WordID> word_ids,
	    std::vector<int> document_frequencies) :
    keywords(keywords), tokens(tokens),
    word_ids(word_ids), document_frequencies(document_frequencies) {
  };
  std::string keywords;
  std::vector<icu::UnicodeString> tokens;
  std::vector<WordID> word_ids;
  std::vector<int> document_frequencies;
};

class QueryProcessor {
public:
  QueryProcessor(Lexicon &&lexicon,
		 icu::Locale locale=icu::Locale::getRoot());
  QueryProcessor(const Lexicon &lexicon,
		 icu::Locale locale=icu::Locale::getRoot());
  QueryProcessor(std::istream &ist,
		 icu::Locale locale=icu::Locale::getRoot());
  QueryInfo processQuery(const std::string &keywords) const;

private:
  Lexicon lexicon_;
  Tokenizer tokenizer_;
};

} // namespace sese

# endif // #ifndef _SESE_QUERY_H_



#include "query.h"

#include <iostream>
#include <vector>

#include "lexicon.h"
#include "tokenizer.h"

namespace sese {

QueryProcessor::QueryProcessor(Lexicon &&lexicon, icu::Locale locale) :
  lexicon_(lexicon), tokenizer_(locale) {
}

QueryProcessor::QueryProcessor(const Lexicon &lexicon, icu::Locale locale) :
  lexicon_(lexicon), tokenizer_(locale) {
}

QueryProcessor::QueryProcessor(std::istream &ist, icu::Locale locale) :
  lexicon_(ist), tokenizer_(locale) {
}

QueryInfo QueryProcessor::processQuery(const std::string &keywords) const {
  std::vector<UnicodeString> tokens = tokenizer_.tokenize(keywords);
  std::vector<WordID> word_ids = lexicon_.tokens2ids(tokens);
  std::vector<int> document_frequencies = lexicon_.ids2DocumentFrequencies(word_ids);
  return QueryInfo(keywords, tokens, word_ids, document_frequencies);
}

} // namespace sese

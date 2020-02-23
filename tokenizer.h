#ifndef _SESS_TOKENIZER_H_
#define _SESS_TOKENIZER_H_

#include <vector>

#include <unicode/rbbi.h>

namespace sese {

// Tokenizes utf-8 string and outputs word sequence as vector<UnicodeString>.
class Tokenizer {
public:
  Tokenizer(icu::Locale locale=icu::Locale::getRoot());
  ~Tokenizer();
  std::vector<UnicodeString> tokenize(std::string str);

private:
  icu::BreakIterator* bi;
};

} // namespace sese

# endif // #ifndef _SESS_TOKENIZER_H_

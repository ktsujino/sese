#ifndef _SESE_TOKENIZER_H_
#define _SESE_TOKENIZER_H_

#include <vector>

#include <unicode/unistr.h>
#include <unicode/rbbi.h>

namespace sese {

// Tokenizes utf-8 string and outputs word sequence as vector<icu::UnicodeString>.
class Tokenizer {
public:
  Tokenizer(icu::Locale locale=icu::Locale::getRoot());
  ~Tokenizer();
  std::vector<icu::UnicodeString> tokenize(std::string utf8str, bool enable_normalize=true) const;
  std::vector<icu::UnicodeString> normalize(std::vector<icu::UnicodeString> tokens) const;

private:
  icu::BreakIterator* bi_;
};

} // namespace sese

# endif // #ifndef _SESE_TOKENIZER_H_

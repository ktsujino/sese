#include "tokenizer.h"

#include <vector>

#include <unicode/rbbi.h>
#include <unicode/ustream.h>

namespace sese {

Tokenizer::Tokenizer(icu::Locale locale) {
  UErrorCode status = U_ZERO_ERROR;
  bi = BreakIterator::createWordInstance(locale, status);
}

Tokenizer::~Tokenizer() {
  delete bi;
}

std::vector<UnicodeString> Tokenizer::tokenize(std::string str) {
  UnicodeString uStr = UnicodeString::fromUTF8(str);
  std::vector<UnicodeString> words;
  
  bi->setText(uStr);
  int32_t pos = bi->first();
  while (pos != BreakIterator::DONE) {
    int32_t nextPos = bi->next();
    if (nextPos > pos) {
      words.push_back(uStr.tempSubString(pos, nextPos - pos));
    }
    pos = nextPos;
  }
  return words;  
}

} // namespace sese


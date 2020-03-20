#ifndef _SESE_RANKER_H
#define _SESE_RANKER_H

#include <vector>

#include "index.h"
#include "query.h"

namespace sese {

using RankScore = double;

class Ranker {
public:
  std::vector<std::pair<MatchInfo, RankScore>> rank(const std::vector<MatchInfo> &documents,
						    const QueryInfo &query_info) const;

private:
  RankScore calcScore(const MatchInfo &match_info,
		      const QueryInfo &query_info) const;

  RankScore tfidfScore(const MatchInfo &match_info,
		       const QueryInfo &query_info) const;
};

} // namespace sese

#endif // #ifndef _SESE_RANKER_H


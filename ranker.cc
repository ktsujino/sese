#include "ranker.h"

#include <cmath>
#include <vector>

#include "index.h"
#include "query.h"

namespace sese {

std::vector<std::pair<MatchInfo, RankScore>> Ranker::rank(const std::vector<MatchInfo> &documents,
							  const QueryInfo &query_info) {
  std::vector<std::pair<MatchInfo, RankScore>> ranked_output;
  for (const MatchInfo &match_info : documents) {
    RankScore score = calcScore(match_info, query_info);
    ranked_output.push_back(std::make_pair(match_info, score));
  }
  std::sort(ranked_output.begin(),
	    ranked_output.end(),
	    [](const std::pair<MatchInfo, RankScore> &left,
	       const std::pair<MatchInfo, RankScore> &right) {
	      return left.second > right.second;
	    });
  return ranked_output;
}

RankScore Ranker::calcScore(const MatchInfo &match_info,
			    const QueryInfo &query_info) {
  return tfidfScore(match_info, query_info);
}

RankScore Ranker::tfidfScore(const MatchInfo &match_info,
			     const QueryInfo &query_info) {
  RankScore score{0};
  for (int pos = 0; pos < match_info.term_frequency.size(); pos++) {
    RankScore tf = static_cast<RankScore>(match_info.term_frequency[pos])
      / static_cast<RankScore>(match_info.document_length);
    RankScore log_df = std::log(static_cast<RankScore>(query_info.document_frequencies[pos]));
    score += tf / log_df;
  }
  return score;
}

} // namespace sese


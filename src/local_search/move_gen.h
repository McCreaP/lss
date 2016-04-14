#ifndef LSS_LOCAL_SEARCH_MOVE_GEN_H_
#define LSS_LOCAL_SEARCH_MOVE_GEN_H_

#include <vector>

#include "local_search/move.h"

namespace lss {
namespace local_search {

class MoveGen {
 public:
  virtual ~MoveGen() = default;

  virtual Move* GetOne() = 0;
  virtual std::vector<Move*> GenN(int n) = 0;

  static MoveGen* Default(const State &state);
};

}  // namespace local_search
}  // namespace lss

#endif  // LSS_LOCAL_SEARCH_MOVE_GEN_H_

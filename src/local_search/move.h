#ifndef LSS_LOCAL_SEARCH_MOVE_H_
#define LSS_LOCAL_SEARCH_MOVE_H_

#include "local_search/state.h"

namespace lss {
namespace local_search {

class Move {
 public:
  virtual void Do(State *s) = 0;
  virtual void Undo(State *) = 0;

  virtual ~Move() = default;
};

}  // namespace local_search
}  // namespace lss

#endif  // LSS_LOCAL_SEARCH_MOVE_H_

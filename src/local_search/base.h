// A temporary header with "mock" definitions.
// To be removed after integration with the rest of the system.

#ifndef LSS_LOCAL_SEARCH_BASE_H_
#define LSS_LOCAL_SEARCH_BASE_H_

namespace lss {
namespace local_search {

// To be replaced with `::lss::Schedule`.
class BaseSchedule {};

// To be replaced with `::lss::Algorithm`.
class BaseAlgorithm {
 public:
  virtual void UpdateSituation(Situation s) = 0;
  virtual Schedule Run() = 0;

  virtual ~BaseAlgorithm() = default;
};

}  // namespace local_search
}  // namespace lss

#endif  // LSS_LOCAL_SEARCH_BASE_H_

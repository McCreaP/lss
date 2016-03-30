#ifndef LSS_ALGORITHM_H_
#define LSS_ALGORITHM_H_

namespace lss {

class Algorithm {
 public:
  virtual Schedule run(const Schedule &prevSchedule, const Situation &situation) = 0;

  virtual ~Algorithm();
};

}  // namespace lss

#endif  // LSS_ALGORITHM_H_

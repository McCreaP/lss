#ifndef LSS_EXPERIMENTAL_BASE_H_
#define LSS_EXPERIMENTAL_BASE_H_

using IdType = int;
using FloatType = double;
using ContextType = int;
using Time = double;

enum class MachineState;

enum class JobState {
  kUnassigned, kScheduled, kPending, kExecuting
};

#endif // LSS_EXPERIMENTAL_BASE_H_

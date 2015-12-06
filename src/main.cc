#include "scheduler/greedy_scheduler.h"

int main() {
  lss::GreedyScheduler scheduler("/home/vagrant/lss/run/");
  scheduler.Schedule();
  return 0;
}

#include "experimental/situation.h"
#include "experimental/raw_input_reader.h"
#include "experimental/raw_output_handler.h"

#include <memory>


int main(int argc, char** argv) {
  RawInputReader input_reader;
  RawOutputHandler output_handler;

  // Configure both classes with SetPath method based on in argv.

  Situation situation;
  Schedule schedule;
  std::unique_ptr<Algorithm> algorithm(Algorithm::CreateByName(""));

  while (true) {
    input_reader.Read();
    output_handler.Read();

    situation = Situation(situation, input_reader.GetInput(), output_handler.GetOutput());
    schedule = Schedule(schedule, situation);
    schedule = algorithm->Run(situation, schedule);

    output_handler.Write(schedule.ToRawOutput());
  }

  return 0;
}
#ifndef LSS_IO_BASIC_INPUT_TEST_H_
#define LSS_IO_BASIC_INPUT_TEST_H_

#include "gtest/gtest.h"
#include <io/raw_input_types.h>
namespace lss {
namespace io {

TEST(IoTest, Primitive) {
  EXPECT_EQ(kContextN,3);
}

}  // namespace io
}  // namespace lss

#endif  // LSS_IO_BASIC_INPUT_TEST_H_

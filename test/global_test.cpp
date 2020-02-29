#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "gamma/window.hpp"
#include "tests/mock_fact.hpp"
#include "tests/utils.hpp"
using namespace ::testing;
using MainMockFactory = MockFactory<MockManager, MockSdlImpl>;

TEST(NoobyCase, test_return_value) {
	MockManager *manager; MockSdlImpl *sdl_impl;
  MainMockFactory factory;
  
	std::unique_ptr<Window> win(new MainWindow(&factory));

	manager = dynamic_cast<MockManager*>(win->get_manager());
	sdl_impl = dynamic_cast<MockSdlImpl*>(win->get_sdl_impl());


  ON_CALL(*sdl_impl, poll_event)
    .WillByDefault(Return(true));
  ON_CALL(*manager, set_state)
    .WillByDefault(Return());
  ON_CALL(*manager, handle_action)
    .WillByDefault(Return(false));

  bool result = win->run();
  ASSERT_EQ(!result, 0);
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

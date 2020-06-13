
TEST_CASE("Check modify of buffer_view_content") {
  std::vector<std::string> ss {
    "Hello world", "True is not False",
  };
  buffer_view b{ss, 0, 0};

  CHECK(b[0][0] == 'H');
  CHECK(b[1][4] == ' ');

  auto string = b[0];
  string[0] = '!';

  CHECK(b[0][0] == '!');
}



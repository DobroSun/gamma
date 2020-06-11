
void test_view() {
  std::vector<std::string> ss {
    "Hello world", "True is not False",
  };
  buffer_view b{ss, 0, 0};

  EQ(b[0][0], 'H');
  EQ(b[1][4], ' ');

  auto string = b[0];
  string[0] = '!';

  EQ(b[0][0], '!');
}



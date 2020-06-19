
void check_eq_string(const gap_buffer<char> &b, const std::string &s) {
  CHECK(b.size() == s.size());

  for(unsigned i = 0; i < b.size(); i++) {
    CHECK(b[i] == s[i]);
  }
}


void check_read_file(const std::string &c) {
  buffer_t buf;
  std::string s;

  std::fstream file{c.c_str()};
  if(!file) {
    return;
  }

  int count = 0;
  while(std::getline(file, s)) {
    gap_buffer<char> tmp;

    gap_buffer_from_string(tmp, s);
    buf.insert(tmp);

    check_eq_string(buf[count], s);
    count++;
  }
  CHECK(count);
}


namespace fs = std::experimental::filesystem;

TEST_CASE("Test read_file") {
  for(const auto& p: fs::recursive_directory_iterator("../")) {
    check_read_file(p.path());
  }
}

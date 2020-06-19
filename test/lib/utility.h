
void inline gap_buffer_from_string(gap_buffer<char> &g, const std::string &s) {
    for(unsigned i = 0; i < s.size(); i++) {
      g.insert(s[i]); 
    }
}

void inline buffer_from_file(buffer_t &b, const std::string &s) {
  std::fstream file{s.c_str()};
  if(!file) {
    CHECK(false);
  }

  std::string input;
  while(std::getline(file, input)) {
    gap_buffer<char> tmp;
    gap_buffer_from_string(tmp, input);
    b.insert(tmp);
  }
}

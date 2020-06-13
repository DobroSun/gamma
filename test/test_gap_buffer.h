/*
void init(gap_buffer<char> &buffer, const char *c) {
  for_each(c) {
    buffer.insert(*it);
  }
}
template<class T>
void print(const T &a) {
  for(auto i: a) {
    if(!i) {
      std::cout << ' ';
    } else {
      std::cout << i;
    }
  }
  std::cout << std::endl;
}


TEST_CASE("Test gap_buffer_insert") {
  gap_buffer<char> buffer;
  const char *c = "Hello world\n\0";
  init(buffer, c);

  auto c_len = strlen(c);

  auto arr = buffer.buf;
  auto gap_len = buffer.gap_len;
  auto gap_start = buffer.pre_len;
  auto post_start = gap_start + gap_len;


  CHECK(arr.size() == c_len+gap_len);
  CHECK(gap_start == 0);
  CHECK(post_start == gap_len);

  CHECK(arr[gap_start+gap_len] == c[0]);
  CHECK(arr[post_start] == c[0]);
  CHECK(arr[post_start+1] == c[1]);
}


TEST_CASE("Test gap_buffer_inc_cursor") {
  gap_buffer<char> buffer;
  const char *c = "Hello world\n\0";
  init(buffer, c);

  auto c_size = strlen(c);

  unsigned i;
  for(i = 0; i < c_size; i++) {
    auto gap_len = buffer.gap_len;
    auto pre_len = buffer.pre_len;

    buffer.inc_cursor();

    auto new_gap_len = buffer.gap_len;
    auto new_pre_len = buffer.pre_len;

    CHECK(gap_len == new_gap_len);
    CHECK(buffer.pre_len == pre_len+1);
    CHECK(buffer.pre_len == buffer.pre_len);
    CHECK(new_pre_len == pre_len+1);
  }

  CHECK(buffer.pre_len == c_size);

  for( ; i < buffer.buf.size(); i++) {
    CHECK((buffer.pre_len + buffer.gap_len) == buffer.buf.size());
  }
}


TEST_CASE("Test gap_buffer_dec&inc_cursor") {
  gap_buffer<char> buffer;

  auto gap_len = buffer.gap_len;

  for(auto i = 0; i < 10; i++) {
    buffer.dec_cursor();
    CHECK(buffer.pre_len == 0);
    CHECK(buffer.gap_len == gap_len);
  }
  for(auto i = 0; i < 10; i++) {
    buffer.inc_cursor();
    CHECK(buffer.pre_len == 0);
    CHECK(buffer.gap_len == gap_len);
  }
}


TEST_CASE("Test gap_buffer_append") {
  gap_buffer<char> buffer;
  const char *c = "Zefirka\0";
  init(buffer, c);
  auto c_size = strlen(c);
  auto &arr = buffer.buf;


  for(unsigned i = 0; i < 1000; i++) {
    auto ch = c[i % c_size];

    buffer.append(ch);
    std::cout << buffer.gap_len << std::endl;

    CHECK(arr[i] == ch);
    CHECK(buffer.pre_len == i+1);
  }
}
*/

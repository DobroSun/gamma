
template<class T>
void init(gap_buffer<T> &buffer, const T *c) {
  for_each(c) {
    buffer.add(*it);
  }
  buffer.move_left_by(buffer.size());
}

template<class T>
void print(const T &a) {
  for(unsigned i = 0; i < a.size(); i++) {
    auto it = a[i];
    if(!it) {
      std::cout << ' ';
    } else {
      std::cout << it;
    }
  }
  std::cout << std::endl;
}



TEST_CASE("Test gap_buffer_insert") {
  gap_buffer<char> buffer;
  const char *c = "Hello world\0";
  auto c_len = strlen(c);

  for(unsigned i = 0; i < c_len; i++) {
    buffer.add(c[i]);
  }
  buffer.move_left_by(buffer.size());


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



TEST_CASE("Test gap_buffer_move_right") {
  gap_buffer<char> buffer;
  const char *c = "Hello world\n\0";
  init(buffer, c);

  auto c_size = strlen(c);

  unsigned i;
  for(i = 0; i < c_size; i++) {
    auto gap_len = buffer.gap_len;
    auto pre_len = buffer.pre_len;

    buffer.move_right();

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


TEST_CASE("Test gap_buffer_move_left&right") {
  gap_buffer<char> buffer;

  auto gap_len = buffer.gap_len;


  for(int i = 0; i < 10; i++) {
    buffer.move_left();
    CHECK(buffer.pre_len == 0);
    CHECK(buffer.gap_len == gap_len);
  }
  for(int i = 0; i < 10; i++) {
    buffer.move_right();
    CHECK(buffer.pre_len == 0);
    CHECK(buffer.gap_len == gap_len);
  }
}


TEST_CASE("Test gap_buffer_add") {
  gap_buffer<char> buffer;
  const char *c = "Zefirka\0";
  init(buffer, c);
  auto c_size = strlen(c);
  auto &arr = buffer.buf;

  for(unsigned i = 0; i < 1000; i++) {
    auto ch = c[i % c_size];
    buffer.add(ch);
    CHECK(arr[i] == ch);
    CHECK(buffer.pre_len == i+1);
  }
}

TEST_CASE("Test gap_buffer_delete") {
  gap_buffer<char> buffer;
  const char *c = "Test gap_buffer_backspace\0";
  init(buffer, c);
  auto c_size = strlen(c);

  auto pre_len = buffer.pre_len;
  auto gap_len = buffer.gap_len;
  for(unsigned i = 0; i < 100; i++) {
    buffer.backspace();
    CHECK(buffer.pre_len == pre_len);
    CHECK(buffer.gap_len == gap_len);
  }
  for(unsigned i = 1; i < c_size; i++) {
    buffer.del();
    CHECK(buffer.gap_len == ++gap_len);
    CHECK(buffer.pre_len == pre_len);
  }
  for(unsigned i = 0; i < 100; i++) {
    buffer.del();
    buffer.backspace();
    CHECK(buffer.gap_len == gap_len);
    CHECK(buffer.pre_len == pre_len);
  }
}

TEST_CASE("Test gap_buffer_backspace") {
  gap_buffer<char> buffer;
  const char *c = "Test gap_buffer_backspace\0";
  init(buffer, c);
  auto c_size = strlen(c);

  auto pre_len = buffer.pre_len;
  auto gap_len = buffer.gap_len;
  for(unsigned i = 0; i < c_size; i++) {
    buffer.move_right();
    buffer.backspace();

    CHECK(buffer.gap_len == ++gap_len);
    CHECK(buffer.pre_len == pre_len);
  }
}

TEST_CASE("Test gap_buffer[]") {
  gap_buffer<int> buffer;
  unsigned size = 6;
  int a[size] = {1, 2, 3, 4, 5, 6};

  for(unsigned i = 0; i < size; i++) {
    buffer.add(a[i]);
  }
  buffer.move_left_by(buffer.size());



  for(unsigned i = 0; i < size; i++) {
    CHECK(buffer[i] == i+1);
  }

  buffer.move_right();

  for(unsigned i = 0; i < size; i++) {
    CHECK(buffer[i] == i+1);
  }

  buffer.move_right();
  buffer.move_right();
  buffer.move_right();
  buffer.move_right();


  for(unsigned i = 0; i < size; i++) {
    CHECK(buffer[i] == i+1);
  }

  buffer.move_left();

  for(unsigned i = 0; i < size; i++) {
    CHECK(buffer[i] == i+1);
  }

  buffer.move_left();
  buffer.move_left();
  buffer.move_left();
  buffer.move_left();
  buffer.move_left();
  buffer.move_left();
  buffer.move_left();

  for(unsigned i = 0; i < size; i++) {
    CHECK(buffer[i] == i+1);
  }

  buffer.move_right();
  buffer.move_right();
  buffer.move_left();

  for(unsigned i = 0; i < size; i++) {
    CHECK(buffer[i] == i+1);
  }

  buffer.move_left();
  buffer.move_right();
  buffer.move_left();

  for(unsigned i = 0; i < size; i++) {
    CHECK(buffer[i] == i+1);
  }
}

TEST_CASE("Test gap_buffer_init") {
  gap_buffer<gap_buffer<int>> b;

  gap_buffer<int> d;
  gap_buffer<int> e;
  gap_buffer<int> c;
  unsigned size = 6;
  for(unsigned i = 0; i < size; i++) {
    d.add(i);
    e.add(i);
    c.add(i);
  }
  d.move_left_by(d.size());
  e.move_left_by(e.size());
  c.move_left_by(c.size());

  d.move_right();
  e.move_right();
  e.move_right();
  e.move_left();

  b.add(d);
  b.add(e);
  b.add(c);
  b.move_left_by(b.size());

  CHECK(b.size() == 3);

  for(unsigned i = 0; i < 3; i++) {
    b.move_right();
    CHECK(b[0].size() == b[1].size());
    CHECK(b[0].size() == 6);
  }
  for(unsigned i = 0; i < 3; i++) {
    b.move_left();
    CHECK(b[0].size() == b[1].size());
    CHECK(b[0].size() == 6);
  }


  for(unsigned i = 0; i < b.size(); i++) {
    for(unsigned j = 0; j < b[i].size(); j++) {
      CHECK(b[i][j] == j);
    }
  }
}

TEST_CASE("Test gap_buffer_clear") {
  gap_buffer<gap_buffer<char>> b;

  gap_buffer<char> e;
  const char *c1 = "Hello world\n";
  init(e, c1);

  gap_buffer<char> d;
  const char *c2 = "Hello world";
  init(d, c2);

  
  b.add(e);
  b.add(d);
  b.move_left_by(b.size());

  b.clear();
}

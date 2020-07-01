
bool true_id(int,int,int) {
  return true;
}
bool false_id(int,int,int) {
  return false;
}

void cursor_down_not_last(buffer_view &b) {
  cursor_down_detail(b, false_id);
}

void cursor_down_last(buffer_view &b) {
  cursor_down_detail(b, true_id);
}



TEST_CASE("Test cursor_down_not_last") {
  buffer_t buffer;
  const char size = 100;
  const char gap_len = buffer.gap_len;
  buffer.resize(size+gap_len);

  fw = 1;

  CHECK(buffer.buf.capacity() == 100+gap_len);
  CHECK(buffer.buf.size() == 100+gap_len);
  CHECK(buffer.size() == size);

  buffer_view b{buffer};

  
  unsigned i = 0;
  for(i = 0; i < size-1; i++) {
    cursor_down_not_last(b);

    CHECK(b.cursor.i == i+1);
    CHECK(b.pre_len() == i+1);
  }

  auto tmp = i;

  for( ; i < size*2; i++) {
    cursor_down_not_last(b);

    // by default all of lines aren't huge.
    // so no need resizing them.

    CHECK(b.cursor.i == tmp);
    CHECK(b.pre_len() == tmp);
  }
}

TEST_CASE("Test cursor_down_last_trivial_next_is_huge") {
  buffer_t buffer;
  const char size = 100;
  const char gap_len = buffer.gap_len;
  buffer.resize(size+gap_len);

  TextLeftBound = -25;
  fw = 1;
  Width = 10;
  // Actual size we need as width of string.
  // So every string with length > 10 is `huge`.


  buffer_view b{buffer};


  for(unsigned i = 0; i < size-1; i++) {
    const auto start = b.start;
    const auto nhuge = 10*i + gap_len + 1;

    b[i+1].resize(nhuge); // as the cursor.i == 0, so next is `huge`.
    cursor_down_last(b);

    CHECK(b.start == start+i+1);
    CHECK(b.cursor.i == i+1);
    CHECK(b.pre_len() == i+1);
  }
}

TEST_CASE("Test cursor_down_last_trivial_first_is_huge") {
  buffer_t buffer;
  const char size = 100;
  const char gap_len = buffer.gap_len;
  buffer.resize(size+gap_len);

  TextLeftBound = -25;
  fw = 1;
  Width = 10;

  buffer_view b{buffer};

  for(unsigned i = 0; i < size-1; i++) {
    const auto start = b.start;
    const auto nhuge = 10*i + gap_len + 1;

    b[start].resize(nhuge);
    cursor_down_last(b);


    CHECK(b.start == i+1);
    CHECK(b.cursor.i == i+1);
    CHECK(b.pre_len() == i+1);
  }
}

TEST_CASE("Test cursor_down_last_non_trivial") {
  buffer_t buffer;
  const char size = 100;
  const char gap_len = buffer.gap_len;
  buffer.resize(size+gap_len);

  TextLeftBound = -25;
  fw = 1;
  Width = 10;

  buffer_view b{buffer};


  auto nhuge = [gap_len](int n) {
    return 10*n + gap_len + 1;
  };
  auto start = b.start;
  auto i = 50; b.cursor.i = i;

// Cases on first_line > next_line.

  b[start].resize(nhuge(2));   // -3 lines.
  b[start+1].resize(nhuge(0)); // -1 line.
  b[i+1].resize(nhuge(1));     // +2 lines.
  b[i+2].resize(nhuge(1));     // +2 lines.


  cursor_down_last(b);
  CHECK(b.start == start+2);
  CHECK(b.cursor.i == i+1);
  CHECK(b.pre_len() == 1); 

  start = b.start;
  i = b.cursor.i;


  b[start].resize(nhuge(3));   // -4 lines.
  b[start+1].resize(nhuge(0)); // -1 line.
//b[i+1].resize(nhuge(1));     // +2 lines. (is already 2 lines).
  b[i+2].resize(nhuge(2));     // +3 lines.

  cursor_down_last(b);
  CHECK(b.start == start+2);
  CHECK(b.cursor.i == i+1);
  CHECK(b.pre_len() == 2);
  
  start = b.start;
  i = b.cursor.i;

  b[start].resize(nhuge(11)); // 12 lines.
  b[i+1].resize(nhuge(5));    // 6 lines.
  b[i+2].resize(nhuge(5));    // 6 lines.

  cursor_down_last(b);

  CHECK(b.start == start+1);
  CHECK(b.cursor.i == i+1);
  CHECK(b.pre_len() == 3);


// Cases on next_line > first_line.
  buffer_t buffer2;
  buffer2.resize(size+gap_len);
  buffer_view v{buffer2};

  v.cursor.i = 10;

  start = v.start;
  i = v.cursor.i;


  v[start].resize(nhuge(1));   // -2 lines.
  v[start+1].resize(nhuge(0)); // -1 line.
  v[start+2].resize(nhuge(0)); // -1 line.
  v[i+1].resize(nhuge(3));     // +4 lines.

  cursor_down_last(v);

  CHECK(v.start == start+3);
  CHECK(v.cursor.i == i+1);
  CHECK(v.pre_len() == 1);


  start = v.start;
  i = v.cursor.i;


  v[start].resize(nhuge(4));    // -5 lines.
  v[start+1].resize(nhuge(10)); // -11 lines.
  v[i+1].resize(nhuge(5));      // +6 lines.
  v[i+2].resize(nhuge(4));      // +5 lines.
  v[i+3].resize(nhuge(4));      // +5 lines.


  cursor_down_last(v);

  CHECK(v.start == start+2);
  CHECK(v.cursor.i == i+1);
  CHECK(v.pre_len() == 2);

  start = 25; v.start = start;
  i = v.cursor.i;

  v[start].resize(nhuge(1));   // 2 lines.
  v[start+1].resize(nhuge(5)); // 6 lines.
  v[start+2].resize(nhuge(2)); // 3 lines.
  v[i+1].resize(nhuge(10));  // 11 lines.

  cursor_down_last(v);

  CHECK(v.start == start+3);
  CHECK(v.cursor.i == i+1);
  CHECK(v.pre_len() == 3);
}

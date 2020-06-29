
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
  const auto pre_len = b.pre_len();

  
  unsigned i = 0;
  for(i = 0; i < size-1; i++) {
    cursor_down_not_last(b);

    CHECK(b.cursor.i == i+1);
    CHECK(b.pre_len() == pre_len+i+1);
  }

  auto tmp = i;
  auto tmp_len = pre_len+i;

  for( ; i < size*2; i++) {
    cursor_down_not_last(b);

    CHECK(b.cursor.i == tmp);
    CHECK(b.pre_len() == tmp_len);
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


  const auto pre_len = b.pre_len();
  for(unsigned i = 0; i < size-1; i++) {
    const auto start = b.start;
    const auto nhuge = 10*i + gap_len + 1;

    b[i+1].resize(nhuge); // as the cursor.i == 0, so next is `huge`.
    cursor_down_last(b);

    CHECK(b.start == start+i+1);
    CHECK(b.cursor.i == i+1);
    CHECK(b.pre_len() == pre_len+i+1);
  }
}

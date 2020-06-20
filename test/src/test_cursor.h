

TEST_CASE("Test cursor moving") {
/*
  buffer_t b;

  buffer_from_file(b, "../test/text.txt");

  
  buffer_view buffer{b, 0};
  Cursor cursor{0,0};

  auto pj = cursor.j;
  for(auto i = 0; i < 10; i++) {
    handle_scroll_up(buffer, cursor);

    CHECK(buffer.start == 0);
    CHECK(cursor.i == 0);
    CHECK(cursor.j == pj);
  }

  // Depends on number of lines in file.
  for(unsigned i = 0; i < 10; i++) {
    handle_scroll_down(buffer, cursor);
    CHECK(buffer.start == (i+1)*scroll_speed);
    CHECK(cursor.i == 0);
    CHECK(cursor.j == pj);
  }


  for(unsigned i = 0; i < 10; i++) {
    handle_scroll_up(buffer, cursor);

    CHECK(buffer.start == (9-i)*scroll_speed);
    CHECK(cursor.j == pj);

    if(i < 6) {
      // Cursor moves to bottom.
      CHECK(cursor.i == (i+1)*scroll_speed);
    } else {
      // Cursor reaches bottom of window, and stops.
      CHECK(cursor.i == 20);
    }
  }
*/
}


TEST_CASE("Test buffer_view") {
  buffer_t buffer;
  gap_buffer<char> g;
  gap_buffer<char> b;

  const char *c = "HEllo world";
  const char *u = "New line";
  for_each(c) {
    g.insert(*it);
  }
  for_each(u) {
    b.insert(*it);
  }


  buffer.insert(g);
  buffer.insert(b);

  CHECK(buffer[0][0] == 'H');
  buffer[0] = b;
  CHECK(buffer[0][0] == 'N');
}


TEST_CASE("Test array default constructor") {
  array<int> a;
  CHECK(a.size() == a.capacity());
  CHECK(a.size() == 0);
}

TEST_CASE("Test array_reserve&resize_empty") {
  array<int> a;
  array<int> b;
    
  auto a_size = a.size();
  for(unsigned i = 1; i < 1000; i++) {
    a.reserve(i);
    CHECK(a.size() == a_size);
    CHECK(a.capacity() == i);

    CHECK(a.size() <= a.capacity());
    CHECK(b.size() <= b.capacity());

    b.resize(i);
    CHECK(b.size() == b.capacity());
    CHECK(b.capacity() == i);
  }
}

TEST_CASE("Test array_reserve&resize_with_elements") {
  array<int> a;

  CHECK(a.size() <= a.capacity());

  for(unsigned i = 0; i < 10; i++) {
    a.push_back(i);
  }
  CHECK(a.size() == 10);
  CHECK(a.capacity() == 16);

  auto size = a.size();
  a.resize(100);
  for(unsigned i = size; i < a.size(); i++) {
    CHECK(a[i] == int{});
  }
}

TEST_CASE("Test array_push_back") {
  array<int> a;

  a.push_back(12);
  CHECK(a.size() == 1);
  CHECK(a.capacity() == 8);
  CHECK(a[0] == 12);

  CHECK(a.size() <= a.capacity());
  for(unsigned i = 2; i < 1000; i++) {
    a.push_back(i);
    CHECK(a.size() == i);
    CHECK(a[i-1] == i);
    CHECK(a.size() <= a.capacity());
  }
}

TEST_CASE("Test array_pop_back") {
  array<int> a;

  for(unsigned i = 0; i < 1000; i++) {
    a.push_back(i);
  }
  auto capacity = a.capacity();
  for(unsigned i = 1000; i > 0; i--) {
    a.pop_back();

    CHECK(a.size() == i-1);
    CHECK(a.capacity() == capacity);
  }
}

TEST_CASE("Test array_front") {
  array<int> a;

  for(unsigned i = 0; i < 1000; i++) {
    a.push_back(i);
    CHECK(a.front() == 0);
  }
}

TEST_CASE("Test array_back") {
  array<int> a;

  for(unsigned i = 0; i < 1000; i++) {
    a.push_back(i);
    CHECK(a.back() == i);
  }
}

TEST_CASE("Test array_clear") {
  array<int> a;
  a.clear();
  CHECK(a.size() == 0);

  a.push_back(10);
  a.clear();
  CHECK(a.size() <= a.capacity());
  CHECK(a.size() == 0);
}

TEST_CASE("Test array_empty") {
  array<int> a;
  CHECK(a.empty() == true);
  a.push_back(1);
  CHECK(a.empty() == false);
}

TEST_CASE("Test array_swap") {
  array<int> a; a.push_back(10);
  array<int> b; b.push_back(0);

  a.swap(b);
  CHECK(a[0] == 0);
  CHECK(b[0] == 10);
}

TEST_CASE("Test array_swap") {
  array<int> a; a.push_back(10);
  array<int> b; b.push_back(0);

  auto res1 = b == a;
  CHECK(res1 == false);

  b[0] = 10;
  auto res2 = b == a;
  CHECK(res2 == true);
}

TEST_CASE("Test array_copy_constructor") {
  array<int> a;

  for(int i = 0; i < 10; i++) {
    a.push_back(i);
  }

  array<int> b{a};
  CHECK((b == a) == true);
  CHECK(b.size() == a.size());
}

TEST_CASE("Test array_copy_assign") {
  array<int> a, b;

  for(int i = 0; i < 10; i++) {
    a.push_back(i);
  }

  b = a;
  CHECK((b == a) == true);
  CHECK(b.size() == a.size());
}

TEST_CASE("Test array_move_constructor") {
  array<int> a, c;
  for(int i = 0; i < 10; i++) {
    a.push_back(i);
  }

  array<int> b {std::move(a)};
  CHECK(a.size() == 0);
  CHECK(a.capacity() == 0);
  CHECK((a == c) == true);
}

TEST_CASE("Test array_move_copy") {
  array<int> a, b, c;

  for(int i = 0; i < 10; i++) {
    b.push_back(i);
    c.push_back(i);
  }
  auto size = b.size();

  a = std::move(b);
  CHECK(b.size() == 0);
  CHECK(b.capacity() == 0);
  CHECK(a.size() == size);
  CHECK(a == c);

  a.clear();
  CHECK(a == b);
}


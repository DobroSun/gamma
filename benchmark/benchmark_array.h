
struct BigStruct {
  double a[100000];
};


static void vector_push_back(benchmark::State &state) {
  for(auto _: state) {
    std::vector<double> v;
    for(int i = 0; i < 100; i++) {
      v.push_back(i);
    }
  }
}

static void array_push_back(benchmark::State &state) {
  for(auto _: state) {
    array<double> a;
    for(int i = 0; i < 100; i++) {
      a.push_back(i);
    }
  }
}



BENCHMARK(vector_push_back);
BENCHMARK(array_push_back);

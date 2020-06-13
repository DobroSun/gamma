#ifndef GAMMA_GAP_BUFFER_H
#define GAMMA_GAP_BUFFER_H

// So I will have buffer type like this:
// buffergap<buffergap<char>> buffer;
/*
Wanna my custom dynamic array type.
template<class T>
class array {
};

*/
/*


template<class T>
struct gap_buffer {
  array<T> buf;
  unsigned pre_len;
  unsigned gap_len;


  gap_buffer(unsigned __pre=0)
    : pre_len{__pre}, gap_len{2} //  TODO: Check on the fastest default value for gap_len.
    {
    buf.resize(gap_len);
  }

  void insert(T val) {
    buf.push_back(std::move(val));
  }

  void inc_cursor() {
    auto post_start = pre_len + gap_len;
    auto gap_start = pre_len;
    if(buf.size() == post_start) {
      return;
    }
    buf[gap_start] = buf[post_start];
    pre_len++;
  }

  void dec_cursor() {
    auto post_start = pre_len + gap_len;
    auto gap_start = pre_len;
    if(gap_start == 0) {
      return;
    }
    buf[post_start] = buf[gap_start];
    pre_len--;
  }

  void append(T val) {
    auto post_start = pre_len + gap_len;
    auto gap_start = pre_len;

    if(gap_len == 0)  {
      Allocate array with new gap.
        1) allocate size + gap_len*2.
        2) copy from new_post_start() to an end, post_len # characters.
      So from pre_len to pre_len+gap_len will be new gap.
      unsigned prev_size = buf.size();

      buf.reserve(prev_size*2);
      gap_len = prev_size-1;

      auto to = post_start + prev_size;
      unsigned i = post_start, j = post_start+prev_size;
      for(int k = j-i; k > -1; k--) {
        buf[to+k] = buf[i+k];
      }
    }

    buf[gap_start] = val;
    pre_len++;
    gap_len--;
  }



private:
};


// [1, 2, [' ', ' '], 3, 4, 5, 6] buffer.
//           gap:2


   buffer_t buffer; // with default cursor == 0,0;

   append();
   pop();

using buffer_t = gap_buffer<gap_buffer<char>>;
*/
#endif

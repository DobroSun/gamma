#ifndef GAMMA_TESTS_UTILS_HPP
#define GAMMA_TESTS_UTILS_HPP

#define PTR_TO_VAL(ptr, mock_type) \
  *dynamic_cast<mock_type*>(ptr)
#endif

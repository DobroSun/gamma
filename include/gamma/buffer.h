#ifndef GAMMA_BUFFER_H
#define GAMMA_BUFFER_H
#include "gamma/fwd_decl.h"

bool load_buffer_from_file(const string &filename);

buffer_view &get_buffer();
#endif

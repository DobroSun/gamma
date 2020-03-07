#ifndef GAMMA_GLOBAL_VARIABLES_HPP
#define GAMMA_GLOBAL_VARIABLES_HPP
#include <string>

const int WIDTH = 1200;
const int HEIGHT = 900;
const int EXITWIDTH = 400;
const int EXITHEIGHT = 200;

struct assets {
  static const std::string path;
};
inline const std::string assets::path = "/gamma/assets/";

#define GREY 128, 128, 128, 255
#define ORANGE 255, 165, 0, 255
#endif

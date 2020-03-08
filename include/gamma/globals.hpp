#ifndef GAMMA_GLOBAL_VARIABLES_HPP
#define GAMMA_GLOBAL_VARIABLES_HPP
#include <string>

const int WIDTH = 1200;
const int HEIGHT = 900;
const int EXITWIDTH = 400;
const int EXITHEIGHT = 200;


// FIXME:
// Fix travis `inline error`
// It cannot declare inline variable. Like this.
// | inline const std::string = "/gamma/assets/";
inline std::string ASSETS_PATH() {
  std::string path = "/gamma/assets/";
  return path;
}
#define assets_path ASSETS_PATH()

#define GREY 128, 128, 128, 255
#define ORANGE 255, 165, 0, 255
#endif

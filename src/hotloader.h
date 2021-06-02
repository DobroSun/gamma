#ifndef GAMMA_HOTLOADER_H
#define GAMMA_HOTLOADER_H

struct Settings_Hotloader {
  int fd;
  bool tries_to_update_second_time;



  Settings_Hotloader(const char *);
  ~Settings_Hotloader();

  bool settings_need_reload();
  void reload_file(const char *);
};

#endif

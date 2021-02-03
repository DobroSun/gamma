#ifndef GAMMA_HOTLOADER_H
#define GAMMA_HOTLOADER_H

struct Settings_Hotloader {
  int fd, wd;

  inotify_event *current_event;
  inotify_event  event_buffer[10];
  bool is_not_reloaded = false;


  Settings_Hotloader(const char *);
  ~Settings_Hotloader();

  bool settings_need_reload();
  void reload_file(const char *);
};

#endif

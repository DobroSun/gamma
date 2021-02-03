#include "pch.h"
#include "hotloader.h"
#include "buffer.h"
#include "interp.h"

Settings_Hotloader::Settings_Hotloader(const char *filename) {
  fd = inotify_init();

  if(fd == -1) { fprintf(stderr, "Inotify init failed!\n"); }

  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	wd = inotify_add_watch(fd, filename, IN_ALL_EVENTS);
  if(wd == -1) { fprintf(stderr, "Failed to add a watch for `syntax.m`\n"); }
}

Settings_Hotloader::~Settings_Hotloader() {
  close(fd);
}

bool Settings_Hotloader::settings_need_reload() {
  if(is_not_reloaded) { return true; } // needs a reload.

  if(read(fd, event_buffer, sizeof(event_buffer)) != -1) {
    current_event = event_buffer;
    if(current_event->mask & IN_MODIFY) {
      return true;
    } else {
      return false;
    }
  }
  return false;
}

void Settings_Hotloader::reload_file(const char *name) {
  FILE *f = fopen(name, "r");
  if(!f) {
    is_not_reloaded = true;
  } else {
    string config;
    {
      defer { fclose(f); };
      read_entire_file(&config, f);
    }
    interp(config.data);
    update_variables();
    is_not_reloaded = false;
  }
}

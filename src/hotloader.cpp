#include "pch.h"
#include "hotloader.h"
#include "buffer.h"
#include "interp.h"

Settings_Hotloader::Settings_Hotloader(const char *name) {
  fd = inotify_init();

  if(fd == -1) { fprintf(stderr, "Inotify init failed!\n"); }

  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	int wd = inotify_add_watch(fd, name, IN_ALL_EVENTS);
  if(wd == -1) { fprintf(stderr, "Failed to add a watch for `syntax.m`\n"); }
}

Settings_Hotloader::~Settings_Hotloader() { close(fd); }


bool Settings_Hotloader::settings_need_reload() {
  inotify_event event;
  if(read(fd, &event, sizeof(event)) != -1) {
    if(event.mask & IN_MODIFY) {
      return true;
    }
  }
  return false;
}

void Settings_Hotloader::reload_file(const char *filename) {
  if(tries_to_update_second_time) {
    tries_to_update_second_time = false;
    return;
  }

  if(FILE *f = fopen(filename, "r")) {
    char *string = NULL;
    defer { if(string) deallocate(string); };

    {
      defer { fclose(f); };
      read_file_into_memory(f, &string);
    }

    interp(string);
    tries_to_update_second_time = true;
  }
}

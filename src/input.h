

void set_input();

void open_console();
void close_console();

void handle_console_keydown(SDL_Keysym);
void handle_insert_mode_keydown(SDL_Keysym);
void handle_visual_mode_keydown(SDL_Keysym);
void handle_normal_mode_keydown(SDL_Keysym);
void handle_tab_mode_keydown(SDL_Keysym);

void to_normal_mode();
void to_insert_mode();
void to_visual_mode();

bool is_normal_mode();
bool is_insert_mode();
bool is_visual_mode();
bool is_visual_line_mode();
bool is_console_mode();

void handle_input_keydown(SDL_Keysym);


-- Constants
EDITOR  = 0
CONSOLE = 1

NORMAL_MODE = 0
INSERT_MODE = 1
VISUAL_MODE = 2


TAB       = '\x09'
BACKSPACE = '\x08'
ESCAPE    = '\x1B'
RETURN    = '\x0D'
DELETE    = '\x7F'
ARROW_RIGHT = 'O'
ARROW_LEFT  = 'P'
ARROW_UP    = 'R'
ARROW_DOWN  = 'Q'
-- end

Height = 500
Width  = 500
tabstop = 2

editor_state = EDITOR
mode         = NORMAL_MODE

function to_normal_mode()
  mode = NORMAL_MODE
  console_clear()
end

function to_insert_mode()
  mode = INSERT_MODE
  console_put_text("-- INSERT --");
end

function to_visual_mode()
  mode = VISUAL_MODE
  console_put_text("-- VISUAL --");
end

function open_console()
  editor_state = CONSOLE
end



function on_d() 
  if mode == NORMAL_MODE then
    -- @Incomplete:
  elseif mode == VISUAL_MODE then
    delete_selected()
    to_normal_mode()
  end
end
function on_h() 
  if mode == NORMAL_MODE then
    go_left(false)
  elseif mode == VISUAL_MODE then
    go_left(true)
  end
end
function on_i() 
  if mode == NORMAL_MODE then
    to_insert_mode()
  end
end
function on_j() 
  if mode == NORMAL_MODE then
    go_down(false)
  elseif mode == VISUAL_MODE then
    go_down(true)
  end
end
function on_k() 
  if mode == NORMAL_MODE then
    go_up(false)
  elseif mode == VISUAL_MODE then
    go_up(true)
  end
end
function on_l() 
  if mode == NORMAL_MODE then
    go_right(false)
  elseif mode == VISUAL_MODE then
    go_right(true)
  end
end
function on_p() 
  if mode == NORMAL_MODE then
    paste_from_global()
  elseif mode == VISUAL_MODE then
    paste_from_global()
  end
end
function on_v() 
  if mode == NORMAL_MODE then
    to_visual_mode()
    start_selection()
  elseif mode == VISUAL_MODE then
    to_normal_mode()
    --end_selection
  end
end
function on_y() 
  if mode == NORMAL_MODE then
    -- @Incomplete:
  elseif mode == VISUAL_MODE then
    copy_selected();
    to_normal_mode()
  end
end



function on_tab()
  if mode == INSERT_MODE then
    for _ = 1,tabstop,1 do
      put(' ')
    end
  end
end
function on_escape()
  if mode == INSERT_MODE then
    to_normal_mode()
  elseif mode == NORMAL_MODE then
    quit() -- @Temporary:
  elseif mode == VISUAL_MODE then
    to_normal_mode()
  end
end
function on_right_arrow()
  if mode == INSERT_MODE or mode == NORMAL_MODE then
    go_right(false)
  elseif mode == VISUAL_MODE then
    go_right(true)
  end
end
function on_left_arrow()
  if mode == INSERT_MODE or mode == NORMAL_MODE then
    go_left(false)
  elseif mode == VISUAL_MODE then
    go_left(true)
  end
end
function on_up_arrow()
  if mode == INSERT_MODE or mode == NORMAL_MODE then
    go_up(false)
  elseif mode == VISUAL_MODE then
    go_up(true)
  end
end
function on_down_arrow()
  if mode == INSERT_MODE or mode == NORMAL_MODE then
    go_down(false)
  elseif mode == VISUAL_MODE then
    go_down(true)
  end
end
function on_return()
  if mode == INSERT_MODE then
    put_return()
  end
end
function on_backspace()
  if mode == INSERT_MODE then
    put_backspace()
  end
end
function on_delete()
  if mode == INSERT_MODE then
    put_delete()
  end
end

function pass()
end

keys = {}
keys['d'] = on_d
keys['h'] = on_h
keys['i'] = on_i
keys['j'] = on_j
keys['k'] = on_k
keys['l'] = on_l
keys['p'] = on_p
keys['v'] = on_v
keys['y'] = on_y
keys[TAB] = on_tab
keys[ESCAPE] = on_escape
keys[ARROW_RIGHT] = on_right_arrow
keys[ARROW_LEFT]  = on_left_arrow
keys[ARROW_UP]    = on_up_arrow
keys[ARROW_DOWN]  = on_down_arrow
keys[RETURN]      = on_return
keys[BACKSPACE]   = on_backspace
keys[DELETE]      = on_delete

shift = {}
shift['s'] = open_console

ctrl = {}
ctrl['s'] = open_console

shift_ctrl = {}

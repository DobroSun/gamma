
-- Constants -- DO NOT EDIT!
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
-- 

Height = 500
Width  = 500
tabstop = 2



editor_state = EDITOR
mode         = NORMAL_MODE

--function min(a, b)  if a < b then return a else return b end end
--function max(a, b)  if a < b then return b else return a end end

function pass() end
function delete_action() end
current_action = pass

function do_action(n, func)
  for _ = 1,n do
    func()
  end
end


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



function on_a()
  if editor_state == EDITOR then
    if mode == NORMAL_MODE then
      go_right()
      to_insert_mode()
    end
  end
end
function on_b()
  if editor_state == EDITOR then
    local n = go_word_backwards()
    if mode == NORMAL_MODE then
      do_action(n, go_left)
    elseif mode == VISUAL_MODE then
      do_action(n, (function ()
                      select_to_left()
                      go_left()
                    end))
    end
  end
end


function on_d() 
  if editor_state == EDITOR then
    if mode == NORMAL_MODE then
      if current_action == delete_action then
        on_0()
        current_action = delete_action
        on_dollar()
        put_delete()

      else
        current_action = delete_action
      end


    elseif mode == VISUAL_MODE then
      delete_selected()
      to_normal_mode()
    end
  end
end
function on_h() 
  if editor_state == EDITOR then
    if mode == NORMAL_MODE then
      if current_action == delete_action then
        put_backspace()
      else
        current_action()
        go_left()
      end
      current_action = pass

    elseif mode == VISUAL_MODE then
      select_to_left()
      go_left()
    end
  end
end
function on_i() 
  if editor_state == EDITOR then
    if mode == NORMAL_MODE then
      to_insert_mode()
    end
  end
end
function on_j()
  if editor_state == EDITOR then
    local n = compute_go_down()
    if mode == NORMAL_MODE then
      if current_action == delete_action then
        do_action(n, put_delete)
      else
        do_action(n, (function () 
                        current_action() 
                        go_right()
                      end))
      end
      current_action = pass

    elseif mode == VISUAL_MODE then
      do_action(n, (function ()
                      select_to_right()
                      go_right() 
                    end))
    end
  end
end
function on_k() 
  if editor_state == EDITOR then
    local n = compute_go_up()
    if mode == NORMAL_MODE then
      if current_action == delete_action then
        do_action(n, put_backspace)
      else
        do_action(n, (function ()
                        current_action()
                        go_left()
                      end))
      end
      current_action = pass

    elseif mode == VISUAL_MODE then
      do_action(n, (function ()
                      select_to_left()
                      go_left()
                    end))
    end
  end
end

function on_l() 
  if editor_state == EDITOR then
    if mode == NORMAL_MODE then
      if current_action == delete_action then
        put_delete()
      else
        current_action()
        go_right()
      end
      current_action = pass

    elseif mode == VISUAL_MODE then
      select_to_right()
      go_right()
    end
  end
end
function on_o() 
  if editor_state == EDITOR then
    if mode == NORMAL_MODE then
      current_action = pass
      on_dollar()

      put_return()
      to_insert_mode()
    end
  end
end
function on_p() 
  if editor_state == EDITOR then
    if mode == NORMAL_MODE then
      paste_from_global()
    elseif mode == VISUAL_MODE then
      paste_from_global()
    end
  end
end
function on_v() 
  if editor_state == EDITOR then
    if mode == NORMAL_MODE then
      to_visual_mode()
      start_selection()
    elseif mode == VISUAL_MODE then
      to_normal_mode()
    end
  end
end
function on_y() 
  if editor_state == EDITOR then
    if mode == NORMAL_MODE then
      -- @Incomplete:
    elseif mode == VISUAL_MODE then
      copy_selected();
      to_normal_mode()
    end
  end
end
function on_w()
  if editor_state == EDITOR then
    local n = go_word_forward()
    if mode == NORMAL_MODE then
      do_action(n, go_right)
    elseif mode == VISUAL_MODE then
      do_action(n, (function ()
                      select_to_right()
                      go_right()
                    end))
    end
  end
end
function on_x()
  if editor_state == EDITOR then
    if mode == NORMAL_MODE then
      put_delete()

    elseif mode == VISUAL_MODE then
      delete_selected()
      to_normal_mode()
    end
  end
end
function on_0()
  if editor_state == EDITOR then
    local n = compute_to_beginning_of_line()
    if mode == NORMAL_MODE then
      if current_action == delete_action then
        do_action(n, put_backspace)
      else
        do_action(n, (function ()
                        current_action()
                        go_left()
                      end))
      end
      current_action = pass

    elseif mode == VISUAL_MODE then
      do_action(n, (function ()
                      select_to_left()
                      go_left()
                    end))
    end
  end
end

function on_space()
  if editor_state == EDITOR then
    if mode == NORMAL_MODE then
      go_right()
    elseif mode == VISUAL_MODE then
      select_to_right()
      go_right()
    end
  end
end
function on_tab()
  if editor_state == EDITOR then
    if mode == INSERT_MODE then
      for _ = 1,tabstop,1 do
        put(' ')
      end
    end
  elseif editor_state == CONSOLE then
    for _ = 1,tabstop,1 do
      console_put(' ')
    end
  end
end
function on_escape()
  if editor_state == EDITOR then
    if mode == INSERT_MODE then
      to_normal_mode()
    elseif mode == NORMAL_MODE then
      quit() -- @Temporary:
    elseif mode == VISUAL_MODE then
      to_normal_mode()
    end
  elseif editor_state == CONSOLE then
    editor_state = EDITOR
  end
end
function on_right_arrow()
  if editor_state == EDITOR then
    if mode == NORMAL_MODE or mode == INSERT_MODE then
      if current_action == delete_action then
        put_delete()
      else
        current_action()
        go_right()
      end
      current_action = pass

    elseif mode == VISUAL_MODE then
      select_to_right()
      go_right()
    end

  elseif editor_state == CONSOLE then
    console_go_right()
  end
end
function on_left_arrow()
  if editor_state == EDITOR then
    if mode == NORMAL_MODE or mode == INSERT_MODE then
      if current_action == delete_action then
        put_backspace()
      else
        current_action()
        go_left()
      end
      current_action = pass

    elseif mode == VISUAL_MODE then
      select_to_left()
      go_left()
    end
  elseif editor_state == CONSOLE then
    console_go_left()
  end
end
function on_up_arrow()
  local n = compute_go_up()
  if mode == NORMAL_MODE or mode == INSERT_MODE then
    if current_action == delete_action then
      do_action(n, put_backspace)
    else
      do_action(n, (function ()
                      current_action()
                      go_left()
                    end))
    end
    current_action = pass

  elseif mode == VISUAL_MODE then
    do_action(n, (function ()
                    select_to_left()
                    go_left()
                  end))
  end
end
function on_down_arrow()
  local n = compute_go_down()
  if mode == NORMAL_MODE or mode == INSERT_MODE then
    if current_action == delete_action then
      do_action(n, put_delete)
    else
      do_action(n, (function ()
                      current_action()
                      go_right()
                    end))
    end
    current_action = pass

  elseif mode == VISUAL_MODE then
    do_action(n, (function ()
                    select_to_right()
                    go_right()
                  end))
  end
end
function on_return()
  if editor_state == EDITOR then
    if mode == INSERT_MODE then
      put_return()
    elseif mode == NORMAL_MODE then
      local n = compute_go_down()
      do_action(n, go_right)

    end
  elseif editor_state == CONSOLE then
    console_eval()
    editor_state = EDITOR
  end
end
function on_backspace()
  if editor_state == EDITOR then
    if mode == INSERT_MODE then
      put_backspace()
    elseif mode == NORMAL_MODE then
      go_left()
    elseif mode == VISUAL_MODE then
      select_to_left()
      go_left()
    end

  elseif editor_state == CONSOLE then
    console_put_backspace()
  end
end
function on_delete()
  if editor_state == EDITOR then
    if mode == INSERT_MODE then
      put_delete()
    end
  elseif editor_state == CONSOLE then
    console_put_delete()
  end
end


keys = {}
keys['a'] = on_a
keys['b'] = on_b
keys['d'] = on_d
keys['h'] = on_h
keys['i'] = on_i
keys['j'] = on_j
keys['k'] = on_k
keys['l'] = on_l
keys['o'] = on_o
keys['p'] = on_p
keys['v'] = on_v
keys['y'] = on_y
keys['w'] = on_w
keys['x'] = on_x
keys['0'] = on_0
keys[' '] = on_space
keys[TAB] = on_tab
keys[ESCAPE] = on_escape
keys[ARROW_RIGHT] = on_right_arrow
keys[ARROW_LEFT]  = on_left_arrow
keys[ARROW_UP]    = on_up_arrow
keys[ARROW_DOWN]  = on_down_arrow
keys[RETURN]      = on_return
keys[BACKSPACE]   = on_backspace
keys[DELETE]      = on_delete


function open_console()
  console_clear()
  editor_state = CONSOLE
end
function on_dollar()
  if editor_state == EDITOR then
    local n = compute_to_end_of_line()
    if mode == NORMAL_MODE then
      if current_action == delete_action then
        do_action(n, put_delete)
      else
        do_action(n, (function ()
                        current_action()
                        go_right()
                      end))
      end
      current_action = pass

    elseif mode == VISUAL_MODE then
      do_action(n, (function ()
                      select_to_right()
                      go_right()
                    end))
    end
  end
end
function on_A()
  if editor_state == EDITOR then
    if mode == NORMAL_MODE then
      current_action = pass
      on_dollar()
      to_insert_mode()

    elseif mode == VISUAL_MODE then
      to_insert_mode()
    end
  end
end
function on_D()
  if editor_state == EDITOR then
    if mode == NORMAL_MODE then
      current_action = delete_action
      on_dollar()
    end
  end
end


shift = {}
shift[';'] = open_console -- ':'
shift['4'] = on_dollar    -- '$'
shift['a'] = on_A         -- 'A'
shift['d'] = on_D         -- 'D'
shift[RETURN] = on_return
shift[BACKSPACE] = on_backspace

ctrl = {}

shift_ctrl = {}

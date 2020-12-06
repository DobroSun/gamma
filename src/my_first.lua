
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



function on_a() 
  if editor_state == EDITOR then
    if mode == INSERT_MODE then
      put('a')
    end

  elseif editor_state == CONSOLE then
    console_put('a')
  end
end
function on_b() 
  if mode == INSERT_MODE then
    put('b')
  end
end
function on_c() 
  if mode == INSERT_MODE then
    put('c')
  end
end
function on_d() 
  if mode == INSERT_MODE then
    put('d')
  elseif mode == NORMAL_MODE then
    -- @Incomplete:
  elseif mode == VISUAL_MODE then
    delete_selected()
    to_normal_mode()
  end
end
function on_e() 
  if mode == INSERT_MODE then
    put('e')
  end
end
function on_f() 
  if mode == INSERT_MODE then
    put('f')
  end
end
function on_g() 
  if mode == INSERT_MODE then
    put('g')
  end
end
function on_h() 
  if mode == INSERT_MODE then
    put('h')
  elseif mode == NORMAL_MODE then
    go_left(false)
  elseif mode == VISUAL_MODE then
    go_left(true)
  end
end
function on_i() 
  if mode == INSERT_MODE then
    put('i')
  elseif mode == NORMAL_MODE then
    to_insert_mode()
  end
end
function on_j() 
  if mode == INSERT_MODE then
    put('j')
  elseif mode == NORMAL_MODE then
    go_down(false)
  elseif mode == VISUAL_MODE then
    go_down(true)
  end
end
function on_k() 
  if mode == INSERT_MODE then
    put('k')
  elseif mode == NORMAL_MODE then
    go_up(false)
  elseif mode == VISUAL_MODE then
    go_up(true)
  end
end
function on_l() 
  if mode == INSERT_MODE then
    put('l')
  elseif mode == NORMAL_MODE then
    go_right(false)
  elseif mode == VISUAL_MODE then
    go_right(true)
  end
end
function on_m() 
  if mode == INSERT_MODE then
    put('m')
  end
end
function on_n() 
  if mode == INSERT_MODE then
    put('n')
  end
end
function on_o() 
  if mode == INSERT_MODE then
    put('o')
  end
end
function on_p() 
  if mode == INSERT_MODE then
    put('p')
  elseif mode == NORMAL_MODE then
    paste_from_global()
  elseif mode == VISUAL_MODE then
    paste_from_global()
  end
end
function on_q() 
  if mode == INSERT_MODE then
    put('q')
  end
end
function on_r() 
  if mode == INSERT_MODE then
    put('r')
  end
end
function on_s() 
  if mode == INSERT_MODE then
    put('s')
  else
    save()
  end
end
function on_t() 
  if mode == INSERT_MODE then
    put('t')
  end
end
function on_u() 
  if mode == INSERT_MODE then
    put('u')
  end
end
function on_v() 
  if mode == INSERT_MODE then
    put('v')
  elseif mode == NORMAL_MODE then
    to_visual_mode()
    start_selection()
  elseif mode == VISUAL_MODE then
    to_normal_mode()
    --end_selection
  end
end
function on_w() 
  if mode == INSERT_MODE then
    put('w')
  end
end
function on_x() 
  if mode == INSERT_MODE then
    put('x')
  end
end
function on_y() 
  if mode == INSERT_MODE then
    put('y')
  elseif mode == NORMAL_MODE then
    -- @Incomplete:
  elseif mode == VISUAL_MODE then
    copy_selected();
    to_normal_mode()
  end
end
function on_z() 
  if mode == INSERT_MODE then
    put('z')
  end
end
function on_0() 
  if mode == INSERT_MODE then
    put('0')
  elseif mode == NORMAL_MODE then
    
  end
end
function on_1() 
  if mode == INSERT_MODE then
    put('1')
  end
end
function on_2() 
  if mode == INSERT_MODE then
    put('2')
  end
end
function on_3() 
  if mode == INSERT_MODE then
    put('3')
  end
end
function on_4() 
  if mode == INSERT_MODE then
    put('4')
  end
end
function on_5() 
  if mode == INSERT_MODE then
    put('5')
  end
end
function on_6() 
  if mode == INSERT_MODE then
    put('6')
  end
end
function on_7() 
  if mode == INSERT_MODE then
    put('7')
  end
end
function on_8() 
  if mode == INSERT_MODE then
    put('8')
  end
end
function on_9() 
  if mode == INSERT_MODE then
    put('9')
  end
end
function on_space()
  if mode == INSERT_MODE then
    put(' ')
  end
end
function on_semicolon()
  if mode == INSERT_MODE then
    put(';')
  end
end
function on_quote()
  if mode == INSERT_MODE then
    put('\'')
  end
end
function on_left_bracket()
  if mode == INSERT_MODE then
    put('[')
  end
end
function on_right_bracket()
  if mode == INSERT_MODE then
    put(']')
  end
end
function on_comma()
  if mode == INSERT_MODE then
    put(',')
  end
end
function on_period()
  if mode == INSERT_MODE then
    put('.')
  end
end
function on_slash()
  if mode == INSERT_MODE then
    put('/')
  end
end
function on_minus()
  if mode == INSERT_MODE then
    put('-')
  end
end
function on_equals()
  if mode == INSERT_MODE then
    put('=')
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
keys['a'] = on_a
keys['b'] = on_b
keys['c'] = on_c
keys['d'] = on_d
keys['e'] = on_e
keys['f'] = on_f
keys['g'] = on_g
keys['h'] = on_h
keys['i'] = on_i
keys['j'] = on_j
keys['k'] = on_k
keys['l'] = on_l
keys['m'] = on_m
keys['n'] = on_n
keys['o'] = on_o
keys['p'] = on_p
keys['q'] = on_q
keys['r'] = on_r
keys['s'] = on_s
keys['t'] = on_t
keys['u'] = on_u
keys['v'] = on_v
keys['w'] = on_w
keys['x'] = on_x
keys['y'] = on_y
keys['z'] = on_z
keys['0'] = on_0
keys['1'] = on_1
keys['2'] = on_2
keys['3'] = on_3
keys['4'] = on_4
keys['5'] = on_5
keys['6'] = on_6
keys['7'] = on_7
keys['8'] = on_8
keys['9'] = on_9
keys[' '] = on_space
keys[';'] = on_semicolon
keys['\''] = on_quote
keys['['] = on_left_bracket
keys[']'] = on_right_bracket
keys[','] = on_comma
keys['.'] = on_period
keys['/'] = on_slash
keys['-'] = on_minus
keys['='] = on_equals
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


font_name "VeraMono.ttf"
font_size 15
background_color  (29, 32, 33)
text_color        (219, 215, 225, 1)
cursor_text_color (29, 32, 33)
cursor_color      (219, 215, 225, 1)
console_color      (29, 32, 33)
console_text_color (219, 215, 225, 1)
searched_text_color  (29, 32, 33)
searched_color       (250, 199, 57, 1)


: syntax ".c" ".h" ".cpp" ".hpp"
const  (254, 128, 25,1)
static (254, 128, 25,1)
extern (254, 128, 25,1)
struct (254, 128, 25,1)
enum   (254, 128, 25,1)

if    (255, 83, 62)
else  (255, 83, 62)
for   (255, 83, 62)
while (255, 83, 62)
new    (255, 83, 62)
delete (255, 83, 62)
return (255, 83, 62)
switch (255, 83, 62)
case   (255, 83, 62)
default (255, 83, 62)
break  (255, 83, 62)
goto   (255, 83, 62)



double (250, 199, 57,1)
int    (250, 199, 57,1)
float  (250, 199, 57,1)
unsigned (250, 199, 57,1)
signed  (250, 199, 57,1)
long    (250, 199, 57,1)
short   (250, 199, 57,1)
char    (250, 199, 57,1)
void    (250, 199, 57,1)
inline  (250, 199, 57,1)
auto    (250, 199, 57,1)
bool    (250, 199, 57,1)
size_t  (250, 199, 57,1)
u8  (250, 199, 57,1)
u16 (250, 199, 57,1)
u32 (250, 199, 57,1)
u64 (250, 199, 57,1)
s8  (250, 199, 57,1)
s16 (250, 199, 57,1)
s32 (250, 199, 57,1)
s64 (250, 199, 57,1)
f32 (250, 199, 57,1)
f64 (250, 199, 57,1)

class    (131, 165, 152,1)
template (131, 165, 152,1)
typename (131, 165, 152,1)

NULL    (211, 134, 155,1)
nullptr (211, 134, 155,1)

: literal (211, 134, 155,1)
: string  (184, 187, 38,1)
: single_line_comment "//" (146, 131, 116) # color??? 
: multi_line_comment  "/*" "*/" 
: end 


: syntax ".py"
def   (255,0,0)
class (255,0,0)
: single_line_comment "#" (105,105,255)
: end

: syntax ".xml"
: single_line_comment "//" (105,105,255)
: multi_line_comment "<!--" "-->"
: end


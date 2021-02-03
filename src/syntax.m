
font_name "Courier-Regular.ttf"
font_size 25

: syntax ".c" ".h" ".cpp" ".hpp"
const  (250,100,0)
static (250,100,0)

if    (255,0,0) 
else  (255,0,0) 
for   (255,0,0) 
while (255,0,0)

double (230,200,0)
int    (230,200,0)
float  (230,200,0)
unsigned (230,200,0)
signed  (230,200,0)
long    (230,200,0)
short   (230,200,0)
char (230,200,0)
void (230,200,0)
inline (230,200,0)
auto (111,111,111)
bool (111,111,111)

struct   (250,100,0)
class    (0,0,250)
template (0,0,250)
typename (0,0,250)


: literal (100,10,255)
: string  (20,100,20)
: single_line_comment "//"  (100, 100, 100) # color???
: multi_line_comment  "/*" "*/" 

: end 


: syntax ".py"
def   (255,0,0)
class (255,0,0)

: single_line_comment "#" (105,105,255)
: end

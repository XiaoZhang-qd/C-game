#include "gui.h"

#include <string.h>

void gui_draw_menu(
ServerInput* input
)
{


DrawText(
"PXPT RACER",
230,
30,
40,
WHITE
);



GuiLabel(
(Rectangle){
120,
90,
100,
30
},
"Name"
);



GuiTextBox(
(Rectangle){
260,
90,
250,
35
},
input->name,
PXPT_MAX_NAME,
input->name_edit
);



GuiLabel(
(Rectangle){
120,
150,
100,
30
},
"Server IP"
);



GuiTextBox(
(Rectangle){
260,
150,
250,
35
},
input->ip,
64,
input->ip_edit
);



GuiLabel(
(Rectangle){
120,
210,
100,
30
},
"Port"
);



GuiTextBox(
(Rectangle){
260,
210,
250,
35
},
input->port,
16,
input->port_edit
);



if(
IsMouseButtonPressed(
MOUSE_BUTTON_LEFT
)
)
{

Vector2 mouse =
GetMousePosition();



if(
CheckCollisionPointRec(
mouse,
(Rectangle){
260,
90,
250,
35
}
)
)
{

input->name_edit=1;
input->ip_edit=0;
input->port_edit=0;
input->error_msg[0]='\0';

}



else if(
CheckCollisionPointRec(
mouse,
(Rectangle){
260,
150,
250,
35
}
)
)
{

input->ip_edit=1;
input->name_edit=0;
input->port_edit=0;
input->error_msg[0]='\0';

}



else if(
CheckCollisionPointRec(
mouse,
(Rectangle){
260,
210,
250,
35
}
)
)
{

input->port_edit=1;
input->name_edit=0;
input->ip_edit=0;
input->error_msg[0]='\0';

}



else
{

input->name_edit=0;
input->ip_edit=0;
input->port_edit=0;

}



}



if(
GuiButton(
(Rectangle){
180,
280,
250,
50
},
"CONNECT"
))
{

input->connect=1;

}



if(
GuiButton(
(Rectangle){
180,
360,
250,
50
},
"CREATE SERVER"
))
{

input->create_server=1;

}



if(
strlen(input->error_msg)
>
0
)
{

DrawText(
input->error_msg,
180,
440,
20,
RED
);

}



}
#include "gui.h"


void gui_draw_menu(
ServerInput* input
)
{


DrawText(
"PXPT RACER",
230,
60,
40,
WHITE
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
150,
250,
35
}
)
)
{

input->ip_edit=1;

input->port_edit=0;

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

input->ip_edit=0;

}



else
{

input->ip_edit=0;

input->port_edit=0;

}



}




if(
GuiButton(
(Rectangle){
180,
300,
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
380,
250,
50
},
"CREATE SERVER"
))
{

input->create_server=1;

}



}
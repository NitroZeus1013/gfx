#shader vertex
#version 330 core
layout(location = 0) in vec4 pos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main(){
    gl_Position = pos;
    TexCoord  = aTexCoord;
}


#shader fragment
#version 330 core

out vec4 FragColor;

uniform vec4 u_Color;

void main(){
        FragColor = u_Color;
}


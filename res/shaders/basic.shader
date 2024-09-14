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
in vec2 TexCoord;

out vec4 FragColor;

void main(){
    if(TexCoord.x <= 0.5){
        FragColor = vec4(1.0,0,0,1.0);
    }else{
        FragColor = vec4(0,0,1.0,1.0);        
    }
}


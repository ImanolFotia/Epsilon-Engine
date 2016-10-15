const char vertex[] =
"#version 330 core \n"
"layout(location=0) in vec3 positions; \n"
"layout(location=1) in vec2 uv; \n"
"uniform vec2 position; \n"
"uniform vec2 scale; \n"
"void main(){ \n"
"gl_Position = vec4(position + (positions.xy * scale), 0.0, 1.0); \n"
"}";

const char fragment[] =
"#version 330 core \n"
"out vec4 FragColor; \n"
"void main(){ \n"
"FragColor = vec4(0.0, 1.0, 0.0, 1.0); \n"
"}";

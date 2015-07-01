#version 150

uniform sampler2DRect tex0;
uniform vec4 colorValue;

in vec2 texCoordVarying;

out vec4 outputColor;

void main(){
    vec4 texel0 = texture(tex0, texCoordVarying);
    outputColor = texel0*colorValue;
}
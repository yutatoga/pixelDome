#version 150

uniform sampler2DRect texR;
uniform sampler2DRect texG;
uniform sampler2DRect texB;
uniform sampler2DRect texMain;

in vec2 texCoordVarying;
out vec4 outputColor;

void main(){
    vec4 txtR = texture(texR, texCoordVarying);
    vec4 txtG = texture(texG, texCoordVarying);
    vec4 txtB = texture(texB, texCoordVarying);
    vec4 mask = texture(texMain, texCoordVarying);

    outputColor = vec4(mask.r*txtR.r, mask.g*txtG.g, mask.b*txtB.b, 1);
}
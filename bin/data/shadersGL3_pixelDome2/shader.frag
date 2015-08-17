#version 150

uniform sampler2DRect texR;
uniform sampler2DRect texG;
uniform sampler2DRect texB;
uniform sampler2DRect texMain;

uniform int colorId;

in vec2 texCoordVarying;
out vec4 outputColor;

void main(){
    vec4 txtR = texture(texR, texCoordVarying);
    vec4 txtG = texture(texG, texCoordVarying);
    vec4 txtB = texture(texB, texCoordVarying);
    vec4 mask = texture(texMain, texCoordVarying);

    switch (colorId){
    	case 1:
	    	outputColor = vec4(txtR.r*mask.r, 0.0, 0.0, 0.3);
    	break;
    	case 2:
	    	outputColor = vec4(0.0, txtG.g*mask.g, 0.0, 0.3);
    	break;
    	case 3:
    		outputColor = vec4(0.0, 0.0, txtB.b*mask.b, 0.3);
    	break;
    	default:
	    	// outputColor = vec4(1.0, 1.0, 1.0, 1.0);
    	break;
    }
}
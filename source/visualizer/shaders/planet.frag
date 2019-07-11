#version 330

in float massF;
uniform sampler1D colorD;

out vec4 fragColor;

void main()
{
    if(massF>1&&massF<1e+30){
        fragColor = texture(colorD, (log(massF)/(log(10)*32))).rgba;
    }
    else if(massF<1){
        fragColor = vec4(0.0,0.0,0.0,1.0);
    }
    else if(massF>1e+30){
        fragColor = vec4(1.0,1.0,0.0,1.0);
    }
}

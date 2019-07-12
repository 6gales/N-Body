#version 330

in float massF;
uniform sampler1D colorD;

out vec4 fragColor;

void main()
{
    if(massF>1e+29){
	  fragColor = texture(colorD, (((log(massF)/(log(10.0)))-29.0)/9.0)).rgba;
    }
    else{
        fragColor = vec4(0.0,0.0,0.0,1.0);
    }
}

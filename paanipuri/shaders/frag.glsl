#version 330 core

in vec3 outColor;
//in vec3 lightDirection;

out vec4 OutColor;

void main()
{
    //Calculate a normal from texture coordinates
    vec3 normal;
    
    normal.xy = gl_PointCoord * 2.0 - vec2(1.0); //this will have to be modified to change the radius
    float mag = dot(normal.xy, normal.xy);
    if(mag > 1.0) discard; //kill any pixels outsize of hte circle
    normal.z = sqrt(1.0 - mag);
    
    //lighting calculation
    //float diffuse = max(0.0, dot(lightDirection, normal));
    
    //OutColor = vec4(outColor, 1) * diffuse;
    OutColor = vec4(outColor,1.0);
}

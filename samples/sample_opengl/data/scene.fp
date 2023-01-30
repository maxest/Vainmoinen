uniform sampler2D diffuseMap;



varying vec3 varColor;
varying vec2 varTexCoord;



void main(void)
{
    gl_FragColor = vec4(varColor, 1.0f);// * texture2D(diffuseMap, varTexCoord);
}

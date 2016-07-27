
uniform const vec2 u_resolution;
uniform const vec4 u_color;
uniform const sampler2D u_texture;
uniform const float u_thickness = 0.005;

// Draws all non-transparent pixels as u_color
void main()
{
	vec2 pos = gl_TexCoord[0].st;
	
	vec4 c = texture2D(u_texture, pos);
	
	// We're not inside the sprite
	if (c.a == 0.0)
	{	
		vec4 pixelLeft = texture2D(u_texture, pos - vec2(u_thickness, 0.0));
		vec4 pixelRight = texture2D(u_texture, pos + vec2(u_thickness, 0.0));
		vec4 pixelTop = texture2D(u_texture, pos - vec2(0.0, u_thickness));
		vec4 pixelBottom = texture2D(u_texture, pos + vec2(0.0, u_thickness));

		// If one of the neighboring pixels is in the sprite, we are an outline
		if ((pixelLeft.a + pixelRight.a + pixelTop.a + pixelBottom.a) > 0.0)
		{
			c = vec4(1.0) *  u_color;
		}	
	}
	
	gl_FragColor = c;
}
precision mediump float;

uniform vec2 u_resolution;		// window resolution
uniform vec2 u_pos;				// top left corner of rectangle
uniform vec2 u_size;			// width and height of rectangle
uniform float u_radius = 30.0;	// corner radius

// All parameters should be in *pixel* units!
float RoundRect(in vec2 distFromCenter, in vec2 halfSize, in float radius)
{
	// One liner:
	return clamp(length(max(abs(distFromCenter) - (halfSize - radius), vec2(0.0))) - radius, 0.0, 1.0);
	
	/* 
		Elaborated version:
	
	vec2 pointInQuadI = abs(distFromCenter); // Rounded rectangles are symmetrical 4 ways, we only need to check in one quadrant
	vec2 halfSizeMinRadius = halfSize - radius;
	vec2 pointInCorner = max(pointInQuadI - halfSizeMinRadius, vec2(0.0)); // Equals either the distFromCenter itself, or (0.0, 0.0) if distFromCenter is not in a corner
	float pointInRoundCorner = length(pointInCorner) - radius; // If the distance to the origin is <= radius, this pixel is in the rounded corner
	
	return clamp(pointInRoundCorner, 0.0, 1.0); 
	*/
}

void main()
{
	vec2 fragCoord = gl_FragCoord.xy;
	fragCoord.y = u_resolution.y - fragCoord.y; // Invert y coords
	
	vec2 halfSize = u_size / 2.0;
	vec2 rectCenter = vec2(u_pos + halfSize);
	
	float pct = RoundRect(fragCoord - rectCenter, halfSize, u_radius);
	
	gl_FragColor = mix(gl_Color, vec4(0.0), pct);
}

precision mediump float;

uniform vec2 u_resolution;
uniform vec4 u_bounds; // In pixel coords
uniform vec2 u_center = vec2(0.5); // in uv coords

const float RADIUS = 0.8;
const float SOFTNESS = 0.6;
const float OPACITY = 0.5;

void main()
{
	vec2 uv = gl_FragCoord.xy / u_resolution.xy;
	
	vec4 rect = u_bounds / vec4(u_resolution.x, u_resolution.y, u_resolution.x, u_resolution.y);
	vec2 pos = (uv - rect.xy) / rect.zw;
	
	float len = length(pos - u_center);
	float vignette = smoothstep(RADIUS, RADIUS - SOFTNESS, len);
	
	gl_FragColor = vec4(mix(gl_Color.rgb, vec3(gl_Color.rgb * vignette), OPACITY), 1.0);	
}


uniform vec2 u_resolution;
uniform vec2 u_position;
uniform vec4 u_color;
uniform float u_radius;
uniform float u_blur;
uniform float u_opacity;

void main()
{
	vec2 pos = u_position.xy;
	pos.y = (u_resolution.y - pos.y);
	const float resLen = length(u_resolution.xy);
	float distFromCenter = distance(gl_FragCoord.xy, pos) / resLen;
	float radius = u_radius / resLen;
	
	const float magic_number = 1.5;
	float dist = magic_number * length(smoothstep(radius, radius + u_blur, distFromCenter));
	vec4 color = vec4(u_color.rgb, mix(u_opacity, 0.0, dist));
	
	gl_FragColor = color;
}
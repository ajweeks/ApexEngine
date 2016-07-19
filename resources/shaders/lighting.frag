
uniform const vec2 u_resolution;
uniform const vec2 u_position;
uniform const vec4 u_color;
uniform const float u_radius;
uniform const float u_blur;
uniform const float u_opacity;

void main()
{
	vec2 pos = u_position.xy;
	pos.y = (u_resolution.y - pos.y);
	float distFromCenter = distance(gl_FragCoord, pos) / length(u_resolution.xy);
	float radius = u_radius / length(u_resolution.xy);
	
	float dist = length(smoothstep(radius, radius + u_blur, distFromCenter));

	vec4 color = mix(vec4(u_color.rgb, u_opacity), vec4(0.0), dist);
	
	gl_FragColor = color;
}
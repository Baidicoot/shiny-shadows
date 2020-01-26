#version 430

in vec4 apparentpos;
in vec4 worldpos;
in vec2 uv;
in vec4 normal;

//uniform int drawID;
uniform sampler2D tex;
uniform vec3 cameraPos;

layout(location = 0) out vec4 posbuff;
layout(location = 1) out vec4 colorbuff;
layout(location = 2) out vec4 normbuff;

vec3 lightPos = vec3(-5, 5, 5);

void main(){
	posbuff = vec4(worldpos.xyz, apparentpos.z);

	vec3 fromLight = normalize(worldpos.xyz - lightPos);
	vec3 toPlayer = normalize(cameraPos - worldpos.xyz);

	vec4 data = texture(tex, uv);

	float diff = dot(normal.xyz, -fromLight);
	float spec = pow(dot(reflect(fromLight, normal.xyz), toPlayer), 32) * 0.5;

	colorbuff.rgb = data.xyz;
	colorbuff.a = diff + spec;
	normbuff = normal;
}
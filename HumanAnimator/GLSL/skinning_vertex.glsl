/**
* $Id: skinning_vertex.glsl
*
* This file used for vertex shader, generate the vertex coordinate of the mesh Model
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*
* The Original Code is Copyright (C) 日本電信電話株式会社 2009
* All rights reserved.
*
*/

const vec4 ambient = {0.15f, 0.15f, 0.15f, 1.f};
const vec4 diffuse = {1.0f, 1.0f, 1.0f, 1.f};
const vec4 specular = {0.8f, 0.8f, 0.8f, 1.f};

const vec3 light0Pos = {5.f, 10.f, 10.f};
const vec3 light1Pos = {-5.f, -10.f, -10.f}; 

uniform bool play;
uniform float mscale;
uniform vec3 toAdd;
uniform vec3 rootoffset;
uniform mat4 refMatrice[40];
uniform mat4 motionMatrice[40];

attribute vec4 position;
attribute vec3 normal;
attribute vec2 texcoord;

attribute vec4 index;
attribute vec4 weight;

varying vec2 uv;
varying vec4 gllighting;

void main()
{
    vec4 transformedPosition = vec4(0.0);
    vec3 transformedNormal = vec3(0.0);

	uv = texcoord;
	gllighting = ambient * 2.f;
    vec4 curIndex = index;
    vec4 curWeight = weight;
    
	if (play)
	{
		for (int i = 0; i < 4; i++)
		{
			int boneID = (int)curIndex.x;
			if (boneID < 0) break;
			
			mat4 m44 = motionMatrice[boneID];
			mat4 invm44 = refMatrice[boneID];
			// transform the offset by bone i
			transformedPosition += (m44 * (invm44 * position) )* curWeight.x;

			mat3 m33 = mat3(m44[0].xyz,
							m44[1].xyz,
							m44[2].xyz);

			// transform normal by bone i
			transformedNormal += m33 * normal * curWeight.x;
			
			curIndex = curIndex.yzwx;
			curWeight = curWeight.yzwx;
		}
		transformedPosition.xyz += rootoffset;
    }
    else
    {
		transformedPosition = position;
		transformedNormal = normal;
	}

	transformedPosition.xyz *= mscale;
	transformedPosition.xyz += toAdd;
	gl_Position = gl_ModelViewProjectionMatrix * transformedPosition;
	transformedNormal = normalize(gl_NormalMatrix * transformedNormal);
	
	vec3 scalePos = transformedPosition.xyz * mscale;
	vec3 light0Vec = normalize(light0Pos - scalePos);
	vec3 light1Vec = normalize(light1Pos - scalePos);
	
	vec3 light0ref = reflect(light0Vec, transformedNormal);
	vec3 light1ref = reflect(light1Vec, transformedNormal);
	
	// calculate diffuse component
    gllighting.rgb += max(dot(transformedNormal, light0Vec), 0.f) * diffuse;
    gllighting.rgb += max(dot(transformedNormal, light1Vec), 0.f) * diffuse;
    
    gllighting.a = 1.f;
    
    // vertex color
    //gl_FrontColor.rgb = gllighting.rgb;
	//gl_FrontColor.a = 1.f;
}
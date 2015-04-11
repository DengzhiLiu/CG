/**
* $Id: skinning_fragment.glsl
*
* This file used for pixel shader, generate the texture and color of the model
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*
* The Original Code is Copyright (C) 日本電信電話株式会社 2009
* All rights reserved.
*
*/

uniform sampler2D tex;
uniform bool usemtl;
uniform bool shadow;

varying vec2 uv;
varying vec4 gllighting;

void main()
{
	// Lookup mesh texture and modulate it with diffuse
	if (shadow)
		gl_FragColor = vec4(0.3f, 0.3f, 0.3f, 1.f);
	else {
		if (usemtl)
			gl_FragColor = gllighting * texture2D(tex, uv);
		else
			gl_FragColor = gllighting;
	}
	//gl_FragColor = gl_Color * texture2D(tex, uv);
}
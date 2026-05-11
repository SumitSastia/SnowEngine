#version 330 core

in vec2 vTexCords;
out vec4 FragColor;

uniform sampler2D screen;
uniform bool normalRender;

const float offset = 1.0 / 300.0;

void main() {

	if( normalRender ) {
		FragColor = texture(screen, vTexCords);
	}
	else{
		// Inversion
		// FragColor = vec4(vec3(1.0 - texture(screen, vTexCords)), 1.0);

		// Grayscale
		// FragColor = texture(screen, vTexCords);
		// float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
		// FragColor = vec4(average, average, average, 1.0);

		// Kernel Effects
		vec2 offsets[9] = vec2[](
		
			vec2(-offset, offset), // top-left
			vec2( 0.0   , offset), // top-center
			vec2( offset, offset), // top-right
			vec2(-offset, 0.0   ), // center-left
			vec2( 0.0   , 0.0   ), // center-center
			vec2( offset, 0.0   ), // center-right
			vec2(-offset,-offset), // bottom-left
			vec2( 0.0   ,-offset), // bottom-center
			vec2( offset,-offset)  // bottom-right
		);

		// Sharpen
		/*float kernel[9] = float[](
			-1,-1,-1,
			-1, 9,-1,
			-1,-1,-1
		);*/

		// Blur
		/*float kernel[9] = float[](
			1.0/16.0 ,2.0/16.0 ,1.0/16.0 ,
			2.0/16.0 ,4.0/16.0 ,2.0/16.0 ,
			1.0/16.0 ,2.0/16.0 ,1.0/16.0 
		);*/

		// Edge Detection
		float kernel[9] = float[](
			1, 1, 1,
			1,-8, 1,
			1, 1, 1
		);

		vec3 sampleTex[9];

		for( int i = 0; i < 9; i++){
			sampleTex[i] = vec3(texture(screen, vTexCords.st + offsets[i]));
		}

		vec3 color = vec3(0.0);

		for( int i = 0; i < 9; i++){
			color += sampleTex[i] * kernel[i];
		}

		FragColor = vec4(color, 1.0);
	}
}
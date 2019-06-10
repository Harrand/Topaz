vec2 parallax_offset(vec2 texcoord)
{
	// MUST normalize this, or everything goes a bit weird.
	vec3 eye_direction_tangentspace = normalize(tbn_matrix * eye_direction_cameraspace);
	vec2 texcoord_offset = eye_direction_tangentspace.xy * (texture2D(parallax_map_sampler, texcoord).r * parallax_multiplier + parallax_bias);
	texcoord_offset.y = -texcoord_offset.y;
	return texcoord_modelspace + texcoord_offset;
}

vec3 diffuse_directional(DirectionalLight light, vec3 diffuse_colour, vec3 normal_cameraspace)
{
	float cos_theta = clamp(dot(normal_cameraspace, light.direction), 0.0, 1.0);
	return diffuse_colour * light.colour * light.power * cos_theta;
}

vec3 diffuse(PointLight light, vec3 diffuse_colour, vec3 normal_cameraspace, vec3 position_cameraspace)
{
	float distance = length(light.position - position_cameraspace);
	DirectionalLight directional;
	directional.direction = light.position + eye_direction_cameraspace;
	directional.colour = light.colour;
	directional.power = light.power;
	return diffuse_directional(directional, diffuse_colour, normal_cameraspace) / pow(distance, 2);
}

vec3 specular_directional(DirectionalLight light, vec3 specular_colour, vec3 normal_cameraspace)
{
	/* //phong specular
	vec3 reflection = reflect(-light.direction, normal_cameraspace);
	float cos_alpha = clamp(dot(eye_direction_cameraspace, reflection), 0.0, 1.0);
	return specular_colour * light.colour * light.power * pow(cos_alpha, 5);
	*/

	//blinn-phong specular
	const float default_shininess = 2;
	float shininess = default_shininess;
	if(has_specular_map)
		shininess = texture(specular_map_sampler, parallax_offset(texcoord_modelspace)).r;
	vec3 halfway_direction = normalize(light.direction + eye_direction_cameraspace);
	float specular_component = pow(max(dot(normal_cameraspace, halfway_direction), 0.0), shininess);
	return specular_colour * light.colour * light.power * specular_component;
}

vec3 specular(PointLight light, vec3 specular_colour, vec3 normal_cameraspace, vec3 position_cameraspace)
{
	float distance = length(light.position - position_cameraspace);
	DirectionalLight directional;
	directional.direction = light.position + eye_direction_cameraspace;
	directional.colour = light.colour;
	directional.power = light.power;
	return specular_directional(directional, specular_colour, normal_cameraspace) / pow(distance, 2);
}

bool in_range(float value)
{
	return value >= 0.0f && value <= 1.0f;
}
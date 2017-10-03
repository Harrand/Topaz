#include "world.hpp"
#include "data.hpp"

World::World(std::string filename, std::string resources_path): filename(std::move(filename)), resources_path(std::move(resources_path))
{
	MDLF input(RawFile(this->filename));
	std::string spawn_point_string = input.getTag("spawnpoint"), spawn_orientation_string = input.getTag("spawnorientation"), gravity_string = input.getTag("gravity");
	if(spawn_point_string != "0" && spawn_orientation_string != "0" && gravity_string != "0")
	{
		this->spawn_point = tz::util::string::vectoriseList3<float>(tz::util::string::deformat(spawn_point_string));
		this->spawn_orientation = tz::util::string::vectoriseList3<float>(tz::util::string::deformat(spawn_orientation_string));
		this->gravity = tz::util::string::vectoriseList3<float>(tz::util::string::deformat(gravity_string));
	}
	else
	{
		this->spawn_point = Vector3F();
		this->spawn_orientation = Vector3F();
		this->gravity = Vector3F();
	}
	std::vector<std::string> object_list = input.getSequence("objects");
	std::vector<std::string> entity_object_list = input.getSequence("entityobjects");
	for(std::string object_name : object_list)
		this->addObject(World::retrieveObjectData(object_name, this->resources_path, input));
	for(std::string entity_object_name : entity_object_list)
		this->addEntityObject(World::retrieveEntityObjectData(entity_object_name, this->resources_path, input));
}

World::World(const World& copy): World(copy.filename){}

World::World(World&& move): filename(move.filename), resources_path(move.resources_path), gravity(move.gravity), spawn_point(move.spawn_point), spawn_orientation(move.spawn_orientation), objects(move.objects), entities(move.entities), entity_objects(std::move(move.entity_objects)), base_lights(std::move(move.base_lights)){}

World::~World()
{
	this->killLights();
}

const std::string& World::getFileName() const
{
	return this->filename;
}

void World::addObject(StaticObject obj)
{
	this->objects.push_back(std::move(obj));
}

void World::addEntity(Entity ent)
{
	if(ent.getForces().find("gravity") != ent.getForces().end())
	{
		ent.removeForce("gravity");
	}
	ent.applyForce("gravity", Force(this->getGravity()));
	this->entities.push_back(std::move(ent));
}

void World::addEntityObject(EntityObject eo)
{
	if(eo.getForces().find("gravity") != eo.getForces().end())
	{
		eo.removeForce("gravity");
	}
	eo.applyForce("gravity", Force(this->getGravity()));
	this->entity_objects.push_back(std::move(eo));
}

void World::addLight(Light light, GLuint shader_program_handle)
{
	while(this->base_lights.size() >= World::MAXIMUM_LIGHTS)
		this->base_lights.erase(this->base_lights.begin());
	this->base_lights[light.getUniforms(shader_program_handle, this->base_lights.size())] = light;
}

void World::setGravity(Vector3F gravity)
{
	this->gravity = gravity;
	for(Entity& ent : this->entities)
	{
		ent.removeForce("gravity");
		ent.applyForce("gravity", Force(this->getGravity()));
	}
	for(EntityObject& eo : this->entity_objects)
	{
		eo.removeForce("gravity");
		eo.applyForce("gravity", Force(this->getGravity()));
	}
}

void World::setSpawnPoint(Vector3F spawn_point)
{
	this->spawn_point = spawn_point;
}

void World::setSpawnOrientation(Vector3F spawn_orientation)
{
	this->spawn_orientation = spawn_orientation;
}

void World::killLights()
{
	for(auto& iter : this->base_lights)
	{
		// Kill all the lights in the shader before losing all the data
		std::vector<float> pos({0.0f, 0.0f, 0.0f}), colour({0.0f, 0.0f, 0.0f});
		glUniform3fv(iter.first.at(0), 1, &(pos[0]));
		glUniform3fv(iter.first.at(1), 1, &(colour[0]));
		glUniform1f(iter.first.at(2), 0);
		glUniform1f(iter.first.at(3), 0);
		glUniform1f(iter.first.at(4), 0);
	}
}

void World::exportWorld(const std::string& world_link) const
{
	const tz::data::Manager data_manager(this->resources_path);
	MDLF output = MDLF(RawFile(world_link));
	output.getRawFile().clear();
	std::vector<std::string> object_list;
	std::vector<std::string> entity_object_list;
	output.deleteSequence("objects");
	output.deleteSequence("entityobjects");
	
	output.editTag("gravity", tz::util::string::format(tz::util::string::devectoriseList3<float>(this->gravity)));
	output.editTag("spawnpoint", tz::util::string::format(tz::util::string::devectoriseList3<float>(this->spawn_point)));
	output.editTag("spawnorientation", tz::util::string::format(tz::util::string::devectoriseList3<float>(this->spawn_orientation)));
	for(std::size_t i = 0; i < this->objects.size(); i++)
	{
		const std::string object_name = "object" + tz::util::cast::toString<float>(i);
		object_list.push_back(object_name);
		const StaticObject current_object = this->objects.at(i);
		
		output.editTag(object_name + ".mesh", data_manager.getResourceName(current_object.getMeshLink()));
		for(auto& texture : current_object.getTextures())
		{
			output.editTag(object_name + ".texture" + tz::util::cast::toString<unsigned int>(static_cast<unsigned int>(texture.second)), data_manager.getResourceName(texture.first));
		}
		output.editTag(object_name + ".pos", tz::util::string::format(tz::util::string::devectoriseList3<float>(current_object.getPosition())));
		output.editTag(object_name + ".rot", tz::util::string::format(tz::util::string::devectoriseList3<float>(current_object.getRotation())));
		output.editTag(object_name + ".scale", tz::util::string::format(tz::util::string::devectoriseList3<float>(current_object.getScale())));
		output.editTag(object_name + ".shininess", tz::util::cast::toString(current_object.getShininess()));
		output.editTag(object_name + ".parallax_map_scale", tz::util::cast::toString(current_object.getParallaxMapScale()));
		output.editTag(object_name + ".parallax_map_offset", tz::util::cast::toString(current_object.getParallaxMapOffset()));
		output.editTag(object_name + ".displacement_factor", tz::util::cast::toString(current_object.getDisplacementFactor()));
	}
	for(std::size_t i = 0; i < this->entity_objects.size(); i++)
	{
		const std::string entity_object_name = "eo" + tz::util::cast::toString<float>(i);
		entity_object_list.push_back(entity_object_name);
		const EntityObject current_entity_object = this->entity_objects.at(i);

		output.editTag(entity_object_name + ".mesh", data_manager.getResourceName(current_entity_object.getMeshLink()));
		for(auto& texture : current_entity_object.getTextures())
		{
			output.editTag(entity_object_name + ".texture" + tz::util::cast::toString<unsigned int>(static_cast<unsigned int>(texture.second)), data_manager.getResourceName(texture.first));
		}
		output.editTag(entity_object_name + ".mass", tz::util::cast::toString<float>(current_entity_object.getMass()));
		output.editTag(entity_object_name + ".pos", tz::util::string::format(tz::util::string::devectoriseList3<float>(current_entity_object.getPosition())));
		output.editTag(entity_object_name + ".rot", tz::util::string::format(tz::util::string::devectoriseList3<float>(current_entity_object.getRotation())));
		output.editTag(entity_object_name + ".scale", tz::util::string::format(tz::util::string::devectoriseList3<float>(current_entity_object.getScale())));
		output.editTag(entity_object_name + ".shininess", tz::util::cast::toString(current_entity_object.getShininess()));
		output.editTag(entity_object_name + ".parallax_map_scale", tz::util::cast::toString(current_entity_object.getParallaxMapScale()));
		output.editTag(entity_object_name + ".parallax_map_offset", tz::util::cast::toString(current_entity_object.getParallaxMapOffset()));
		output.editTag(entity_object_name + ".displacement_factor", tz::util::cast::toString(current_entity_object.getDisplacementFactor()));
	}
	output.addSequence("objects", object_list);
	output.addSequence("entityobjects", entity_object_list);
}

void World::render(Camera& cam, const Shader& shader, unsigned int widata_managerh, unsigned int height, const std::vector<std::unique_ptr<Mesh>>& all_meshes, const std::vector<std::unique_ptr<Texture>>& all_textures, const std::vector<std::unique_ptr<NormalMap>>& all_normalmaps, const std::vector<std::unique_ptr<ParallaxMap>>& all_parallaxmaps, const std::vector<std::unique_ptr<DisplacementMap>>& all_displacementmaps)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	for(auto& obj : this->objects)
	{
		Mesh* mesh = tz::graphics::findMesh(obj.getMeshLink(), all_meshes);
		Texture* tex = nullptr; NormalMap* nm = nullptr; ParallaxMap* pm = nullptr; DisplacementMap* dm = nullptr;
		for(auto& texture : obj.getTextures())
		{
			if(texture.second == Texture::TextureType::TEXTURE)
				tex = Texture::getFromLink<Texture>(texture.first, all_textures);
			else if(texture.second == Texture::TextureType::NORMAL_MAP)
				nm = Texture::getFromLink<NormalMap>(texture.first, all_normalmaps);
			else if(texture.second == Texture::TextureType::PARALLAX_MAP)
				pm = Texture::getFromLink<ParallaxMap>(texture.first, all_parallaxmaps);
			else if(texture.second == Texture::TextureType::DISPLACEMENT_MAP)
				dm = Texture::getFromLink<DisplacementMap>(texture.first, all_displacementmaps);
		}
		obj.render(mesh, tex, nm, pm, dm, cam, shader, widata_managerh, height);
	}
	for(auto& eo : this->entity_objects)
	{
		Mesh* mesh = tz::graphics::findMesh(eo.getMeshLink(), all_meshes);
		Texture* tex = nullptr; NormalMap* nm = nullptr; ParallaxMap* pm = nullptr; DisplacementMap* dm = nullptr;
		for(auto& texture : eo.getTextures())
		{
			if(texture.second == Texture::TextureType::TEXTURE)
				tex = Texture::getFromLink<Texture>(texture.first, all_textures);
			else if(texture.second == Texture::TextureType::NORMAL_MAP)
				nm = Texture::getFromLink<NormalMap>(texture.first, all_normalmaps);
			else if(texture.second == Texture::TextureType::PARALLAX_MAP)
				pm = Texture::getFromLink<ParallaxMap>(texture.first, all_parallaxmaps);
			else if(texture.second == Texture::TextureType::DISPLACEMENT_MAP)
				dm = Texture::getFromLink<DisplacementMap>(texture.first, all_displacementmaps);
		}
		eo.render(mesh, tex, nm, pm, dm, cam, shader, widata_managerh, height);
	}	
	for(auto& iter : this->base_lights)
	{
		Light light = iter.second;
		std::vector<float> pos, colour;
		pos.push_back(light.getPosition().getX());
		pos.push_back(light.getPosition().getY());
		pos.push_back(light.getPosition().getZ());
		colour.push_back(light.getColour().getX());
		colour.push_back(light.getColour().getY());
		colour.push_back(light.getColour().getZ());
		glUniform3fv(iter.first.at(0), 1, &(pos[0]));
		glUniform3fv(iter.first.at(1), 1, &(colour[0]));
		glUniform1f(iter.first.at(2), light.getPower());
		glUniform1f(iter.first.at(3), light.getDiffuseComponent());
		glUniform1f(iter.first.at(4), light.getSpecularComponent());
	}
}

void World::update(unsigned int tps)
{
	for(auto& eo : this->entity_objects)
		eo.updateMotion(tps);
	for(auto& ent : this->entities)
		ent.updateMotion(tps);
}

std::size_t World::getSize() const
{
	return this->objects.size() + this->entities.size() + this->entity_objects.size();
}

const std::vector<StaticObject>& World::getObjects() const
{
	return this->objects;
}

const std::vector<Entity>& World::getEntities() const
{
	return this->entities;
}

const std::vector<EntityObject>& World::getEntityObjects() const
{
	return this->entity_objects;
}

std::vector<StaticObject>& World::getObjectsR()
{
	return this->objects;
}

std::vector<Entity>& World::getEntitiesR()
{
	return this->entities;
}

std::vector<EntityObject>& World::getEntityObjectsR()
{
	return this->entity_objects;
}

const Vector3F& World::getGravity() const
{
	return this->gravity;
}

const Vector3F& World::getSpawnPoint() const
{
	return this->spawn_point;
}

const Vector3F& World::getSpawnOrientation() const
{
	return this->spawn_orientation;
}

Vector3F& World::getGravityR()
{
	return this->gravity;
}

Vector3F& World::getSpawnPointR()
{
	return this->spawn_point;
}

Vector3F& World::getSpawnOrientationR()
{
	return this->spawn_orientation;
}

const std::string& World::getWorldLink() const
{
	return this->filename;
}

const std::map<std::array<GLint, tz::graphics::light_number_of_uniforms>, Light>& World::getLights() const
{
	return this->base_lights;
}

std::map<std::array<GLint, tz::graphics::light_number_of_uniforms>, Light>& World::getLightsR()
{
	return this->base_lights;
}

StaticObject World::retrieveObjectData(const std::string& object_name, std::string resources_path, MDLF& mdlf)
{
	std::string mesh_name = mdlf.getTag(object_name + ".mesh");

	std::string position_string = mdlf.getTag(object_name + ".pos");
	std::string rotation_string = mdlf.getTag(object_name + ".rot");
	std::string scale_string = mdlf.getTag(object_name + ".scale");
	unsigned int shininess = tz::util::cast::fromString<unsigned int>(mdlf.getTag(object_name + ".shininess"));
	float parallax_map_scale = tz::util::cast::fromString<float>(mdlf.getTag(object_name + ".parallax_map_scale"));
	float parallax_map_offset = tz::util::cast::fromString<float>(mdlf.getTag(object_name + ".parallax_map_offset"));
	float displacement_factor = tz::util::cast::fromString<float>(mdlf.getTag(object_name + ".displacement_factor"));
	if(!mdlf.existsTag(object_name + ".shininess"))
		shininess = 5;
	if(!mdlf.existsTag(object_name + ".parallax_map_scale"))
		parallax_map_scale = 0.04f;
	if(!mdlf.existsTag(object_name + ".parallax_map_offset"))
		parallax_map_offset = -0.5f;
	if(!mdlf.existsTag(object_name + ".displacement_factor"))
		displacement_factor = 0.25f;
	tz::data::Manager data_manager(resources_path);
	
	std::string mesh_link = data_manager.getResourceLink(mesh_name);
	std::vector<std::pair<std::string, Texture::TextureType>> textures;
	for(unsigned int i = 0; i < static_cast<unsigned int>(Texture::TextureType::TEXTURE_TYPES); i++)
	{
		std::string texture_name = mdlf.getTag(object_name + ".texture" + tz::util::cast::toString(i));
		std::string texture_link = data_manager.getResourceLink(texture_name);
		textures.emplace_back(texture_link, static_cast<Texture::TextureType>(i));
	}
	return {mesh_link, textures, tz::util::string::vectoriseList3<float>(tz::util::string::deformat(position_string)), tz::util::string::vectoriseList3<float>(tz::util::string::deformat(rotation_string)), tz::util::string::vectoriseList3<float>(tz::util::string::deformat(scale_string)), shininess, parallax_map_scale, parallax_map_offset, displacement_factor};
}

EntityObject World::retrieveEntityObjectData(const std::string& entity_object_name, std::string resources_path, MDLF& mdlf)
{
	std::string mesh_name = mdlf.getTag(entity_object_name + ".mesh");
	std::string mass_string = mdlf.getTag(entity_object_name + ".mass");
	std::string position_string = mdlf.getTag(entity_object_name + ".pos");
	std::string rotation_string = mdlf.getTag(entity_object_name + ".rot");
	std::string scale_string = mdlf.getTag(entity_object_name + ".scale");
	unsigned int shininess = tz::util::cast::fromString<unsigned int>(mdlf.getTag(entity_object_name + ".shininess"));
	float parallax_map_scale = tz::util::cast::fromString<float>(mdlf.getTag(entity_object_name + ".parallax_map_scale"));
	float parallax_map_offset = tz::util::cast::fromString<float>(mdlf.getTag(entity_object_name + ".parallax_map_offset"));
	float displacement_factor = tz::util::cast::fromString<float>(mdlf.getTag(entity_object_name + ".displacement_factor"));
	if(!mdlf.existsTag(entity_object_name + ".shininess"))
		shininess = 5;
	if(!mdlf.existsTag(entity_object_name + ".parallax_map_scale"))
		parallax_map_scale = 0.04f;
	if(!mdlf.existsTag(entity_object_name + ".parallax_map_offset"))
		parallax_map_offset = -0.5f;
	if(!mdlf.existsTag(entity_object_name + ".displacement_factor"))
		displacement_factor = 0.25f;
	
	tz::data::Manager data_manager(resources_path);
	
	std::string mesh_link = data_manager.getResourceLink(mesh_name);
	std::vector<std::pair<std::string, Texture::TextureType>> textures;
	for(unsigned int i = 0; i < static_cast<unsigned int>(Texture::TextureType::TEXTURE_TYPES); i++)
	{
		std::string texture_name = mdlf.getTag(entity_object_name + ".texture" + tz::util::cast::toString(i));
		std::string texture_link = data_manager.getResourceLink(texture_name);
		//tz::util::log::message(entity_object_name, ".texture", tz::util::cast::toString<unsigned int>(i), " yields the name ", texture_name, " and the link ", texture_link, "\n");
		textures.emplace_back(texture_link, static_cast<Texture::TextureType>(i));
	}
	float mass = tz::util::cast::fromString<float>(mass_string);
	
	return {mesh_link, textures, mass, tz::util::string::vectoriseList3<float>(tz::util::string::deformat(position_string)), tz::util::string::vectoriseList3<float>(tz::util::string::deformat(rotation_string)), tz::util::string::vectoriseList3<float>(tz::util::string::deformat(scale_string)), shininess, parallax_map_scale, parallax_map_offset, displacement_factor};
}
#ifndef DATATRANSLATION_HPP
#define DATATRANSLATION_HPP
#include "utility.hpp"
#include "mdl.hpp"
#include <map>
#define RES_POINT std::string("../../../res/runtime")

class DataTranslation
{
public:
	DataTranslation(std::string datafilename);

	std::string getResourceLink(std::string resourceName) const;
	std::string getResourceName(std::string resourceLink) const;
	
	std::map<std::string, std::string> retrieveModels() const;
	std::map<std::string, std::string> retrieveTextures() const;
	std::map<std::string, std::string> retrieveNormalMaps() const;
	std::map<std::string, std::string> retrieveDisplacementMaps() const;
private:
	const std::string datafilename;
};

#endif
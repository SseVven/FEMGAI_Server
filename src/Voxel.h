#pragma once
#include <string>
#include <vector>

class Voxel
{
public:
	Voxel(std::vector<bool> faceStatus);
	~Voxel();
	
	const std::vector<bool>& getFaceStatus() { return faceStatus; };
private:
	//std::string material;
	std::vector<bool> faceStatus;
};




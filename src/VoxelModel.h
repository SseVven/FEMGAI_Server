#pragma once
#include "MyMath.h"
#include <vector>

class VoxelModel
{
public:
	const static double PRECISION;
	const static double HALF_PRECISION;
	enum ModelType {
		BOX = 0x1,
		CONE = 0x2,
		CYLINDER = 0x4,
		SPHERE = 0x8,
		BOOLEAN = 0x10,
	};
	enum FaceDirect {
		LEFT = 0,
		RIGHT = 1,
		FRONT = 2,
		BACK = 3,
		UP = 4,
		DOWN = 5,
	};
public:
	VoxelModel(ModelType type);
	VoxelModel(const std::string&);
	~VoxelModel();

	void setPointsFacesByVoxelData();
	void addPoints(Vector3);
	void addFaces(FaceDirect, unsigned int);

	std::string to_string();
private:
	ModelType type;
	Vector3 center;
	std::vector<Vector3> points;
	std::vector<std::vector<unsigned int>> faces;
	VoxelMap* voxelMap;
};


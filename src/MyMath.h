#pragma once
#include <unordered_map>
#include "Voxel.h"
#include <functional>

class MyMath
{
};

class Vector3
{
private:
	double m_x;
	double m_y;
	double m_z;

public:
	Vector3();
	Vector3(double x, double y, double z);
	Vector3(const Vector3& vec);
	~Vector3();
	double x() const { return m_x; };
	double y() const { return m_y; };
	double z() const { return m_z; };

	const Vector3& operator+(const Vector3& vec);
	const Vector3& operator+=(const Vector3& vec);
	const Vector3& operator-(const Vector3& vec);
	const Vector3& operator-=(const Vector3& vec);
	bool operator==(const Vector3& vec) const;
	bool operator!=(const Vector3& vec) const;
};

class VoxelMap
{
public:
	VoxelMap();
	~VoxelMap();

	bool hasPoint(const Vector3&);
	bool addPoint(const Vector3&, Voxel*);
	bool delPoint(const Vector3&);

	void forEach(std::function<void(Voxel*, Vector3)>);
private:
	std::unordered_map<double, std::unordered_map<double, std::unordered_map<double, Voxel*>>> m_map;
};



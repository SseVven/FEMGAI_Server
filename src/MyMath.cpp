#include "MyMath.h"




Vector3::Vector3()
	:m_x(0), m_y(0), m_z(0)
{
}
Vector3::Vector3(double x, double y, double z)
	:m_x(x), m_y(y), m_z(z)
{
}

Vector3::Vector3(const Vector3& vec)
	:m_x(vec.x()), m_y(vec.y()), m_z(vec.z())
{
}

const Vector3& Vector3::operator+(const Vector3& vec) {
	Vector3 tmp = *this;
	tmp.m_x += vec.m_x;
	tmp.m_y += vec.m_y;
	tmp.m_z += vec.m_z;
	return tmp;
};

const Vector3& Vector3::operator-(const Vector3& vec) {
	Vector3 tmp = *this;
	tmp.m_x -= vec.m_x;
	tmp.m_y -= vec.m_y;
	tmp.m_z -= vec.m_z;
	return tmp;
};

const Vector3& Vector3::operator+=(const Vector3& vec) {
	this->m_x += vec.m_x;
	this->m_y += vec.m_y;
	this->m_z += vec.m_z;
	return *this;
};

const Vector3& Vector3::operator-=(const Vector3& vec) {
	this->m_x -= vec.m_x;
	this->m_y -= vec.m_y;
	this->m_z -= vec.m_z;
	return *this;
};

bool Vector3::operator==(const Vector3& vec) const {
	return this->m_x == vec.m_x && this->m_y == vec.m_y && this->m_z == vec.m_z;
};

bool Vector3::operator!=(const Vector3& vec) const {
	return !(*this == vec);
};

Vector3::~Vector3()
{
}



VoxelMap::VoxelMap()
	:m_map(std::unordered_map<double, std::unordered_map<double, std::unordered_map<double, Voxel*>>>())
{
}

VoxelMap::~VoxelMap()
{
	forEach([](Voxel* voxel, Vector3) {
		delete voxel;
	});
}

bool VoxelMap::hasPoint(const Vector3& vec) {
	return m_map.count(vec.x()) && m_map[vec.x()].count(vec.y()) && m_map[vec.x()][vec.y()].count(vec.z());
}

bool VoxelMap::addPoint(const Vector3& vec, Voxel* voxel) {
	m_map[vec.x()][vec.y()][vec.z()] = voxel;
	return true;
}
bool VoxelMap::delPoint(const Vector3& vec) {
	if (this->hasPoint(vec)) {
		delete m_map[vec.x()][vec.y()][vec.z()];
		m_map[vec.x()][vec.y()].erase(vec.z());
		if (!(m_map[vec.x()][vec.y()].size())) {
			m_map[vec.x()].erase(vec.y());
			if (!(m_map[vec.x()].size()))
				m_map.erase(vec.x());
		}
		return true;
	}
	return false;
}

//std::unordered_map<double, std::unordered_map<double, std::unordered_map<double, Voxel*>>> m_map;

void VoxelMap::forEach(std::function<void(Voxel*, Vector3)> fn) {
	for (auto& a : m_map) {
		double x = a.first;
		std::unordered_map<double, std::unordered_map<double, Voxel*>> XMap = a.second;
		for (auto& b : XMap) {
			double y = b.first;
			std::unordered_map<double, Voxel*> YMap = b.second;
			for (auto& c : YMap) {
				double z = c.first;
				Voxel* voxel = c.second;

				fn(voxel, {x,y,z});
			}
		}
	}
}
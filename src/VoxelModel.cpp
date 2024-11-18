#include "VoxelModel.h"
#include "json/json.h"
#include <iostream>

const double VoxelModel::PRECISION = 0.01; // 1mm
const double VoxelModel::HALF_PRECISION = PRECISION /2;

VoxelModel::VoxelModel(ModelType type)
	:type(type), center(Vector3()), points(std::vector<Vector3>()), faces(std::vector<std::vector<unsigned int>>()), voxelMap(new VoxelMap())
{
}
VoxelModel::VoxelModel(const std::string& body)
	:type(ModelType::BOX), center(Vector3()), points(std::vector<Vector3>()), faces(std::vector<std::vector<unsigned int>>()), voxelMap(new VoxelMap())
{
	Json::Reader reader;
	Json::Value value;

	if (reader.parse(body, value)) {
		type = ModelType(value["type"].asInt());
		center = Vector3(value["params"]["center"][0].asDouble(), value["params"]["center"][1].asDouble(), value["params"]["center"][2].asDouble());

		if (type & ModelType::BOX) {
			const double XLen = value["params"]["XLen"].asDouble();
			const double YLen = value["params"]["YLen"].asDouble();
			const double ZLen = value["params"]["ZLen"].asDouble();

			for (double i = HALF_PRECISION; i < XLen; i += PRECISION) {
				const double iAdd = i+ PRECISION;
				const bool iFirst = i == HALF_PRECISION;
				for (double j = HALF_PRECISION; j < YLen; j += PRECISION) {
					const double jAdd = j + PRECISION;
					const bool jFirst = j == HALF_PRECISION;
					for (double k = HALF_PRECISION; k < ZLen; k += PRECISION) {
						std::vector<bool> out = { iFirst,//L
							k == HALF_PRECISION, // B
							iAdd >= XLen, //R
							k + PRECISION >= ZLen, // F
							jAdd >= YLen, // U
							jFirst }; // D

						if(out[0] || out[1] || out[2] || out[3] || out[4] || out[5])
							this->voxelMap->addPoint({ i,j,k }, new Voxel(out));
					}
				}
			}
		}
		else if (type & ModelType::CONE) {
			const double Height = value["params"]["height"].asDouble();
			const double Radius = value["params"]["radius"].asDouble();
			for (double i = HALF_PRECISION; i < Height; i += PRECISION) {
				const double YLimit = Radius * (1 - i / Height);
				const double powYLimit = pow(YLimit, 2);
				const bool iFirst = i == HALF_PRECISION;
				const double iAdd = pow(Radius * (1 - (i + PRECISION) / Height), 2);
				for (double j = HALF_PRECISION; j < YLimit; j += PRECISION) {
					const double powJ = j * j;
					const double ZLimit = sqrt(powYLimit - powJ);
					const double jAdd = pow(j + PRECISION, 2);
					for (double k = HALF_PRECISION; k < ZLimit; k += PRECISION) {
						std::vector<bool> out = { iFirst,//L
							jAdd + k * k >= powYLimit, // U
							k + PRECISION >= ZLimit, //F
							powJ + k * k >= iAdd }; // R

						if (out[0] || out[1] || out[2] || out[3]) {
							this->voxelMap->addPoint({ i,j,k }, new Voxel({ out[0], false, out[3], out[2], out[1], false }));
							this->voxelMap->addPoint({ i,j,-k }, new Voxel({ out[0], out[2], out[3], false, out[1], false }));
							this->voxelMap->addPoint({ i,-j,k }, new Voxel({ out[0], false, out[3], out[2], false, out[1] }));
							this->voxelMap->addPoint({ i,-j,-k }, new Voxel({ out[0], out[2], out[3], false, false, out[1] }));
						}

					}
				}
			}
		}
		else if (type & ModelType::CYLINDER) {

		}
		else if (type & ModelType::SPHERE) {

		}
		else if (type & ModelType::BOOLEAN) {

		}

		setPointsFacesByVoxelData();
	}
}

// 根据 voxelMap 生成边界点、面
void VoxelModel::setPointsFacesByVoxelData() {
	voxelMap->forEach([&](Voxel* voxel, Vector3 point) {
		std::vector<bool> faceStatus = voxel->getFaceStatus();
		const unsigned int add = points.size();

		int cnt = 0;
		for (int i = 0; i < 6; i++) {
			if (faceStatus[i]) {
				addFaces(FaceDirect(i), add);
				cnt++;
			}
		}
		if (cnt)
			addPoints(point);
	});
}
void VoxelModel::addPoints(Vector3 pos) {
	const double i = pos.x();
	const double j = pos.y();
	const double k = pos.z();
	points.push_back({ i - HALF_PRECISION, j - HALF_PRECISION, k - HALF_PRECISION });
	points.push_back({ i + HALF_PRECISION, j - HALF_PRECISION, k - HALF_PRECISION });
	points.push_back({ i + HALF_PRECISION, j + HALF_PRECISION, k - HALF_PRECISION });
	points.push_back({ i - HALF_PRECISION, j + HALF_PRECISION, k - HALF_PRECISION });
	points.push_back({ i - HALF_PRECISION, j - HALF_PRECISION, k + HALF_PRECISION });
	points.push_back({ i + HALF_PRECISION, j - HALF_PRECISION, k + HALF_PRECISION });
	points.push_back({ i + HALF_PRECISION, j + HALF_PRECISION, k + HALF_PRECISION });
	points.push_back({ i - HALF_PRECISION, j + HALF_PRECISION, k + HALF_PRECISION });
}
void VoxelModel::addFaces(FaceDirect direct, unsigned int add) {
	switch (direct) {
	case FaceDirect::LEFT:
		faces.push_back({4 + add, 7 + add, 3 + add, 0 + add});
		break;
	case FaceDirect::BACK:
		faces.push_back({ 0 + add, 1 + add, 2 + add, 3 + add });
		break;
	case FaceDirect::RIGHT:
		faces.push_back({ 1 + add, 2 + add, 6 + add, 5 + add });
		break;
	case FaceDirect::FRONT:
		faces.push_back({ 4 + add, 5 + add, 6 + add, 7 + add });
		break;
	case FaceDirect::UP:
		faces.push_back({ 7 + add, 6 + add, 2 + add, 3 + add });
		break;
	case FaceDirect::DOWN:
		faces.push_back({ 4 + add, 5 + add, 1 + add, 0 + add });
		break;
	default:
		break;
	}
}

std::string VoxelModel::to_string() {
	Json::Value data;
	Json::Value JPoints;
	Json::Value JPoint;
	Json::Value JFaces;
	Json::Value JFace;

	for (Vector3& p : points) {
		JPoint[0] = p.x();
		JPoint[1] = p.y();
		JPoint[2] = p.z();

		JPoints.append(JPoint);
	}

	for (auto& f : faces) {
		for (int i = 0; i < 4; i++)
			JFace[i] = f[i];

		JFaces.append(JFace);
	}
	data["points"] = JPoints;
	data["faces"] = JFaces;

	return data.toStyledString();
}

VoxelModel::~VoxelModel()
{
	delete voxelMap;
}
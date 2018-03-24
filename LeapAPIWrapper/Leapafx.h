#pragma once

#include <leap.h>
#include "GLMTypes.h"
#include <Windows.h>
#include <map>
#include <glm/gtx/vector_angle.hpp>
#include <array>

// Aliases
using BoneType =	Leap::Bone::Type;
using FingerType =	Leap::Finger::Type;
using BonePair =	std::pair<BoneType, BoneType>;
using LeapVector =	Leap::Vector;

template<class obj>
inline void CleanDelete(obj *& objectToDelete) {

	if (objectToDelete != nullptr) {

		delete objectToDelete;
		objectToDelete = nullptr;

	}

}

namespace Leapafx {

	const int FINGERPOSE_DATASIZE = 10;
	const int FINGER_COUNT = 5;
	const int LEAP_IMG_W = 640;
	const int LEAP_IMG_H = 240;
	const int LEAP_IMAGE_TOTALSIZE = LEAP_IMG_H * LEAP_IMG_W;

	static std::map<FingerType, std::string> FingerNames {

		{ FingerType::TYPE_THUMB,	"Thumb" },
		{ FingerType::TYPE_INDEX,	"Index" },
		{ FingerType::TYPE_MIDDLE,	"Middle" },
		{ FingerType::TYPE_RING,	"Ring" },
		{ FingerType::TYPE_PINKY,	"Pinky" }

	};

	// Array of bones to read per finger. Bone Dictionary for true fingers
	static const std::vector<Leap::Bone::Type> FingerBones {

		Leap::Bone::TYPE_METACARPAL,
		Leap::Bone::TYPE_PROXIMAL,
		Leap::Bone::TYPE_INTERMEDIATE,
		Leap::Bone::TYPE_DISTAL

	};

	// Array of bones to read per thumb. Bone Dictionary for thumb finger objects
	static const std::vector<Leap::Bone::Type> ThumbBones {

		Leap::Bone::TYPE_PROXIMAL,
		Leap::Bone::TYPE_INTERMEDIATE,
		Leap::Bone::TYPE_DISTAL

	};

	// Fingers per hand (iterable)
	static const std::vector<Leap::Finger::Type> HandFingers{

		Leap::Finger::Type::TYPE_THUMB,
		Leap::Finger::Type::TYPE_INDEX,
		Leap::Finger::Type::TYPE_MIDDLE,
		Leap::Finger::Type::TYPE_RING,
		Leap::Finger::Type::TYPE_PINKY

	};

	// Pairs of bones in a (true) finger to compare for bend measurement
	static const std::vector<BonePair> FingerBonePairs {

		std::make_pair<BoneType, BoneType>(Leap::Bone::Type::TYPE_METACARPAL,	Leap::Bone::Type::TYPE_PROXIMAL),
		std::make_pair<BoneType, BoneType>(Leap::Bone::Type::TYPE_PROXIMAL,		Leap::Bone::Type::TYPE_INTERMEDIATE),
		std::make_pair<BoneType, BoneType>(Leap::Bone::Type::TYPE_INTERMEDIATE, Leap::Bone::Type::TYPE_DISTAL)
	
	};

	// Pairs of bones in a (thumb) finger to compare for bend measurement
	static const std::vector<BonePair> ThumbBonePairs {

		std::make_pair<BoneType, BoneType>(Leap::Bone::Type::TYPE_METACARPAL,	Leap::Bone::Type::TYPE_PROXIMAL),
		std::make_pair<BoneType, BoneType>(Leap::Bone::Type::TYPE_PROXIMAL,		Leap::Bone::Type::TYPE_INTERMEDIATE),
		std::make_pair<BoneType, BoneType>(Leap::Bone::Type::TYPE_INTERMEDIATE,	Leap::Bone::Type::TYPE_DISTAL)

	};

}

// Dominant/nondominant enum
enum class Handedness {
	Dominant,
	NonDominant
};

// Enumerate left or right (or invalid=NONE)
enum class HANDTYPE {

	NONE = 0,
	LEFT = 1,
	RIGHT = 2

};

// Enumerate Joints
enum class HANDJOINT {

	META_PROX = 10,
	PROX_INTR = 20,
	INTR_DIST = 30

};

// Enumerate Leap Cameras (stereo view, has left/right camera)
enum class LEAPCAMERA {

	LEFT = 0,	// Left lens
	RIGHT = 1	// Right lens

};

// Enumerate the point on the bone
enum class BONEPOINT {

	PROX = 0,	// 'near' end or 'base'
	MID = 1,	// Midpoint of bone
	DIST = 2	// 'far' end or 'tip'

};

// Leap Joint smoothing types - currently only passthrough (raw)
enum class LeapJointReaderType {

	Passthrough = 0,
	Other = 1

};

// Build a LONG/DWORD (unique) from the [Hand Type (left/right)] [finger type], [bone type], [Bone point]
inline long HandEntityID(HANDTYPE hType, FingerType fType, BoneType bType, BONEPOINT bPoint) {

	// Make a long (DWORD from params)
	return MAKELONG(

		MAKEWORD(static_cast<BYTE>(fType), static_cast<BYTE>(bType)),
		MAKEWORD(static_cast<BYTE>(bPoint), static_cast<BYTE>(hType))

	);

}

// Convert leap vector to Vec3
inline Vec3 toVec3(Leap::Vector v) {

	return Vec3(v.x, v.y, v.z);

}

// Return a triplet of angles a direction vector uses
inline Vec3 directionToAngles(Vec3 vec) {

	return Vec3(
		glm::angle(vec, Vec3(1.0f, 0.0f, 0.0f)),
		glm::angle(vec, Vec3(0.0f, 1.0f, 0.0f)),
		glm::angle(vec, Vec3(0.0f, 0.0f, 1.0f))
	);

}

// Inverse Signed angle between bones
inline float BoneAngleSigned(HANDTYPE handedness, Leap::Bone& boneA, Leap::Bone& boneB) {

	// Angle between bone A&B Z-Basis
	float rawAngle = boneA.basis().zBasis.angleTo(boneB.basis().zBasis);

	// Cross product
	LeapVector CrossBones = boneA.basis().zBasis.cross(boneB.basis().zBasis);

	// X-Basis of bone (according to hand's LCS handedness - left- or right-handed)
	LeapVector boneXBasis = (handedness == HANDTYPE::LEFT)? boneA.basis().xBasis.backward() : boneA.basis().xBasis;

	// return signed angle
	return (CrossBones.dot(boneXBasis) >= 0) ? rawAngle * 1.0f : rawAngle;

}

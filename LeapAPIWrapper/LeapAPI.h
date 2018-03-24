#pragma once

#include "Leapafx.h"
#include "essentials.h"

// include library file via code (as opposed to VS settings)
#pragma comment (lib, "Leap.lib")

class Material {};

class LeapAPI{

	private:

		static Leap::Controller s_mController;

		static bool				s_Status;
		static bool				s_isHMDMode;
		static bool				s_HasNewData;
		static int64_t			s_lastFrameID;
		static Leap::Frame		s_currentFrame;
		static Leap::Device		s_currentDevice;
		static UINT8*			s_CamImage_Left_Raw;	// Raw camera image data (left)
		static UINT8*			s_CamImage_Right_Raw;	// Raw camera image data (right)
		static UINT32*			s_CamImage_Left_RGBA;	// RGBA Camera texture data (left)
		static UINT32*			s_CamImage_Right_RGBA;	// RGBA Camera texture data (right)
		static Leap::Hand		s_BlankHand;

		static int		GetLeapDeviceCount() { return s_mController.devices().count(); }
		static bool		GetLeftHand(Leap::Frame& srcFrame, Leap::Hand & HandOut);
		static bool		GetRightHand(Leap::Frame& srcFrame, Leap::Hand & HandOut);
		static bool		LeftHandDetected(Leap::Frame& srcFrame);
		static bool		RightHandDetected(Leap::Frame& srcFrame);
		static bool		GetFrame(Leap::Frame& dest);
		static bool		GetFrame(Leap::Frame& dest, int historyIndex);
		static void		SetFrameDataDirty();
		static void		SetFrameDataClean();
		static void		SetFrameIndex() { s_lastFrameID = s_currentFrame.id(); }
		static void		SetFrameIndex(int64_t value) { s_lastFrameID = value; }
		static void		UpdateStatus(bool verbose);
		static void		SetWorkingStatus(bool value);

	public:

		LeapAPI(const LeapAPI&) = delete;
		LeapAPI& operator=(const LeapAPI&) = delete;

		// Main logic
		static bool		Initialize();
		static void		Shutdown();
		static void		Update(float dTime, float wTime);
		static bool		HasNewData() { return s_HasNewData; }
		static bool		GetHandData(HANDTYPE type, Leap::Hand & HandOut);	// Extract hand data from the API
		static bool		RenderTo(LEAPCAMERA cam, Material* mtl);

		// AntTweakbar<-->LeapAPI callbacks
		static bool		LeftHandDetected();
		static bool		RightHandDetected();
		static bool		StatusIsWorking();
		static int		HandsDetectedCount() { return s_currentFrame.hands().count(); }
		static void		SetSensorHMDMode(bool HMD);
		static bool		GetSensorHMDMode() { return s_isHMDMode; }

};

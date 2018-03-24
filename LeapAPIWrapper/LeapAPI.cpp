#include "LeapAPI.h"

using namespace Leap;

bool			LeapAPI::s_isHMDMode{ false };		// HMD mode
bool			LeapAPI::s_HasNewData{ false };		// Data ready (clean/dirty) 
bool			LeapAPI::s_Status{ false };
UINT8*			LeapAPI::s_CamImage_Left_Raw;	// scratch memory for camera images
UINT8*			LeapAPI::s_CamImage_Right_Raw;	// scratch memory for camera images
UINT32*			LeapAPI::s_CamImage_Left_RGBA;	// RGBA Camera texture data (left)
UINT32*			LeapAPI::s_CamImage_Right_RGBA;	// RGBA Camera texture data (right)
int64_t			LeapAPI::s_lastFrameID{ -1 };		// ID of last frame grabbed
Leap::Device	LeapAPI::s_currentDevice{ Leap::Device() };	// Leap device
Controller		LeapAPI::s_mController;				// Interface to controller/sensor
Frame			LeapAPI::s_currentFrame{ Frame() };	// Store the latest frame
Leap::Hand		LeapAPI::s_BlankHand{ Leap::Hand() };

// Initialization
bool LeapAPI::Initialize() {

	// Enable "background frames" - the ability to read sensor data even if the current Window does not have 'focus'
	s_mController.setPolicy(Controller::POLICY_BACKGROUND_FRAMES);

	// Enable image extraction from sensor/camera
	s_mController.setPolicy(Leap::Controller::POLICY_IMAGES);

	// Get the status of the device/API
	UpdateStatus(true);

	// Create camera image scratch buffer
	s_CamImage_Left_Raw = new UINT8[Leapafx::LEAP_IMAGE_TOTALSIZE];
	s_CamImage_Right_Raw = new UINT8[Leapafx::LEAP_IMAGE_TOTALSIZE];

	// Create camera image texture RGBA buffer
	s_CamImage_Left_RGBA = new UINT32[Leapafx::LEAP_IMAGE_TOTALSIZE];
	s_CamImage_Right_RGBA = new UINT32[Leapafx::LEAP_IMAGE_TOTALSIZE];

	// Status should function
	return StatusIsWorking();

}

// Shutdown (replacement for dtor)
void LeapAPI::Shutdown() {

	// Clear camera buffers
	CleanDelete(s_CamImage_Left_Raw);
	CleanDelete(s_CamImage_Right_Raw);
	CleanDelete(s_CamImage_Left_RGBA);
	CleanDelete(s_CamImage_Right_RGBA);

}

// Update any members or data
void LeapAPI::Update(float dTime, float wTime) {

	// target pointer for storing camera data
	UINT8* camDataTarget{ nullptr };

	// Update the status of the device/API
	UpdateStatus(false);

	// Check for new data, flag as appropriate
	s_HasNewData = (s_mController.frame().id() != s_lastFrameID);

	// Initialize current frame
	s_currentFrame = s_mController.frame();

	// ---- Get the image data ----
	// Get the image list from the current leap frame
	Leap::ImageList currentList = s_currentFrame.images();

	// For both images in the list
	for (unsigned int i = 0; i < 2; i++) {

		// Get the current image
		Leap::Image currentImg = currentList[i];

		// Check validity
		if (currentImg.isValid()) {

			// Set target for data
			camDataTarget = (i == static_cast<int>(LEAPCAMERA::LEFT)) ? s_CamImage_Left_Raw : s_CamImage_Right_Raw;

			// Store
			const unsigned char* imgData = currentImg.data();
			if (imgData) {

				memcpy(camDataTarget, imgData, sizeof(unsigned char) * currentImg.height() * currentImg.width() * currentImg.bytesPerPixel());

			}

		}
		else {

			// Extracted camera image is not valid

		}

	}

}

// Return a left hand (if available)
bool LeapAPI::GetLeftHand(Leap::Frame& srcFrame, Leap::Hand & HandOut) {

	// Invalid "blank" hand as default
	HandOut = s_BlankHand;

	// This simply returns the first left hand in the hands list. No other checks are performed
	if (LeftHandDetected()) {

		HandList handsInFrame = srcFrame.hands();

		// Cycle the hand list for a left hand
		for (auto listedHand : handsInFrame) {

			// Left hand? Return it
			if (listedHand.isLeft()) {

				// Only return if valid
				if (listedHand.isValid()) {

					HandOut = listedHand;

					return true;
				}

			}

		}

	}

	// No success, no write (hand state stays the same)
	return false;

}

// Return a right hand (if available)
bool LeapAPI::GetRightHand(Leap::Frame& srcFrame, Leap::Hand & HandOut) {

	// Invalid "blank" hand as default
	HandOut = s_BlankHand;

	// This simply returns the first right hand in the hands list. No other checks are performed
	if (RightHandDetected()) {

		HandList handsInFrame = srcFrame.hands();

		// Cycle the hand list for a left hand

		for (auto listedHand : handsInFrame) {

			// Left hand? Return it
			if (listedHand.isRight()) {

				HandOut = listedHand;

				return true;

			}

		}

	}

	return false;
}

bool LeapAPI::LeftHandDetected(Leap::Frame & srcFrame) {

	bool result{ false };

	HandList handsInFrame = srcFrame.hands();

	// cycle the list for a left hand

	for (auto listedHand : handsInFrame) {

		result |= listedHand.isLeft();

	}

	return result;
}

bool LeapAPI::RightHandDetected(Leap::Frame & srcFrame) {

	bool result{ false };

	HandList handsInFrame = srcFrame.hands();

	// cycle the list for a left hand

	for (auto listedHand : handsInFrame) {

		result |= listedHand.isRight();

	}

	return result;

}

// Get the latest frame from the sensor. TRUE indicates a valid frame
bool LeapAPI::GetFrame(Leap::Frame & dest) {

	// Get new frame from sensor
	dest = s_mController.frame();

	// Set the frame index as the latest frame
	SetFrameIndex();

	// This data is now used
	SetFrameDataDirty();

	return dest.isValid();

}

// Get a previous frame from the sensor. TRUE indicates a valid frame
bool LeapAPI::GetFrame(Leap::Frame & dest, int historyIndex) {

	// Get new frame from sensor
	dest = s_mController.frame(historyIndex);

	return dest.isValid();

}

// the data is used
void LeapAPI::SetFrameDataDirty() {
	
	s_HasNewData = false;

}

// The data is clean
void LeapAPI::SetFrameDataClean() {
	
	s_HasNewData = false;

}

// Update the status of the controller/API
void LeapAPI::UpdateStatus(bool verbose) {

	// Get operating status from API
	bool bConnected = s_mController.isConnected();
	bool bSvcConnected = s_mController.isServiceConnected();

	// Output warnings if requested
	if (verbose) {

		if (!bConnected) {

			// Controller not connected
			Shutdown();

		}

		if (!bSvcConnected) {

			// Leap Service not available
			Shutdown();

		}

	}

	// Set
	SetWorkingStatus(bConnected && bSvcConnected);

}

// Set the status flag
void LeapAPI::SetWorkingStatus(bool value) {

	// set
	s_Status = value;

}

// Render/copy a leap camera image to a material
bool LeapAPI::RenderTo(LEAPCAMERA cam, Material * mtl) {

	// Check frame validity
	if (!s_currentFrame.isValid() && s_currentFrame.id() != LeapAPI::s_lastFrameID) {

		// Error message is dealt with by other parts of LeapAPI object

		return false;

	}

	// Get the source buffer to read
	UINT8* srcBuffer = (cam == LEAPCAMERA::LEFT) ? s_CamImage_Left_Raw : s_CamImage_Right_Raw;
	UINT32* targetBuffer = (cam == LEAPCAMERA::LEFT) ? s_CamImage_Left_RGBA : s_CamImage_Right_RGBA;

	// Convert the data to RGBA quad (full alpha)
	for (unsigned int i = 0; i < Leapafx::LEAP_IMAGE_TOTALSIZE; i++) {

		UINT8 value = srcBuffer[i];
		targetBuffer[i] = static_cast<UINT32>(MAKELONG(MAKEWORD(value, value), MAKEWORD(value,255)));

	}

	// Update texture
	// ... code here to copy to texture

	return true;

}

// Is the left hand in frame and detected?
bool LeapAPI::LeftHandDetected() {

	bool result{ false };

	HandList handsInFrame = s_currentFrame.hands();

	// cycle the list for a left hand

	for (auto listedHand : handsInFrame) {

		result |= listedHand.isLeft();

	}

	return result;
}

// Is the right hand in frame and detected?
bool LeapAPI::RightHandDetected() {

	bool result{ false };

	HandList handsInFrame = s_currentFrame.hands();

	// cycle the list for a right hand

	for (auto listedHand : handsInFrame) {

		result |= listedHand.isRight();

	}

	return result;
}

bool LeapAPI::StatusIsWorking() {

	return s_Status;

}

// Enable/Disable HMD mode
void LeapAPI::SetSensorHMDMode(bool HMD) {

	s_isHMDMode = HMD;

	if (HMD) {
		
		// Enable
		s_mController.setPolicy(Controller::POLICY_OPTIMIZE_HMD);

	} else {

		// Disable
		s_mController.clearPolicy(Controller::POLICY_OPTIMIZE_HMD);

	}
}

// return given hand date (left or right)
bool LeapAPI::GetHandData(HANDTYPE type, Hand & HandOut) {

	// If a new frame exists, set the current frame as this frame
	if (HasNewData()) {

		// Get the new frame
		GetFrame(s_currentFrame);

	}

	// Get left hand
	if (type == HANDTYPE::LEFT) {

		return GetLeftHand(s_currentFrame, HandOut);

	}


	// Get right hand
	if (type == HANDTYPE::RIGHT) {

		return GetRightHand(s_currentFrame, HandOut);

	}

	// if failure, or NONE type
	return false;
}

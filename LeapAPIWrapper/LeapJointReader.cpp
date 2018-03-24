#include "LeapJointReader.h"

// Get latest hand data for [type] hand
bool LeapJointReader::GetSensorData(HANDTYPE type, Leap::Hand & HandOut) {

	// Get hand
	LeapAPI::GetHandData(type, HandOut);

	// successful
	return true;
}

// Get the data from the reader (processed) - the output map
bool LeapJointReader::GetData(std::map<long, Vec3>& dest) {
	
	return false;

}

// Update raw data with new API data and perform processing
void LeapJointReader::Update(float dTime) {

	// Update raw states (get from API)
	GetSensorData(m_handType, m_rawHand);

	// Perform processing (read from rawHand, new data in processedHand)
	Process(m_rawHand, m_processedHand);

}

// Reset data (zero)
void LeapJointReader::Reset() {

	// Reset hand objects
	m_rawHand = Leap::Hand();
	m_processedHand = Leap::Hand();

}

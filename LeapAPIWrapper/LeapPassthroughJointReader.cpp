#include "LeapPassthroughJointReader.h"

bool LeapPassthroughJointReader::Process(Leap::Hand & src, Leap::Hand & dest) {

	// (passthrough - no processing, copy raw/in data to processed/out)
	CopyHandData(src, dest);

	return true;

}

LeapPassthroughJointReader::~LeapPassthroughJointReader() { }

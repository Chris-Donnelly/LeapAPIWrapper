#pragma once
#include "LeapJointReader.h"

class LeapPassthroughJointReader : 	public LeapJointReader {

	protected:

		// Passthrough reader is a simply in/raw-->out/processed data copy
		virtual bool Process(Leap::Hand& src, Leap::Hand& dest) final override;

	public:

		LeapPassthroughJointReader(HANDTYPE type) { m_handType = type; }	// cannot use base member in CTOR init list
		virtual ~LeapPassthroughJointReader();

};

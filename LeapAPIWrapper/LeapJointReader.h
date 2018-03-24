#pragma once

#include "essentials.h"
#include "LeapAPI.h"

class LeapJointReader {

	protected:

		virtual bool GetSensorData(HANDTYPE type, Leap::Hand & HandOut);
		virtual bool Process(Leap::Hand& src, Leap::Hand& dest) = 0;

		virtual void CopyHandData(const Leap::Hand& src, Leap::Hand& dest) { dest = src; };

		HANDTYPE	m_handType;
		
		Leap::Hand	m_rawHand{ Leap::Hand() };
		Leap::Hand	m_processedHand{ Leap::Hand() };

	public:

		LeapJointReader::LeapJointReader() {};
		virtual ~LeapJointReader() {};
		virtual void Reset();

		virtual bool GetData(std::map<long, Vec3>& dest);
		virtual void Update(float dTime);

		virtual void GetHandData(Leap::Hand& dest) { CopyHandData(m_processedHand, dest); }
		virtual void GetRawHandData(Leap::Hand& dest) { CopyHandData(m_rawHand, dest); }

};

//---------------------------------------------------------------------------

#ifndef CapabilitiesH
#define CapabilitiesH
//---------------------------------------------------------------------------

#include "..\miniscope\device\MiniscopeDeviceCapabilities.h"

class Capabilities
{
private:
	struct S_SCOPE_CAPABILITIES caps;
	bool signalInverted;
	int signalOffset;
	int bitsPerSample;
public:

	enum { MAX_SENSITIVITY_RANGES = 32 };
	Capabilities(void);
	struct S_SCOPE_CAPABILITIES& get(void);

	E_TRIGGER_MODE getSafeTriggerMode(int mode);
	E_TRIGGER_TYPE getSafeTriggerType(int type);
	E_COUPLING_TYPE getSafeCouplingType(int type);
	int getSafeBufferSize(unsigned int size);

	bool isSignalInverted(void) {
		return signalInverted;
	}
	int getSignalOffset(void) {
        return signalOffset;
    }
};

extern Capabilities capabilities;

#endif

#include "ofxACNSender.h"

#ifdef WIN32
#include <iphlpapi.h>
#include <codecvt>
#else
#include <ifaddrs.h>
#endif

void ofxACNSender::setup(std::string addr, bool bCast)
{
    ipAddress = addr;
    bMcast = bCast;
    setLengthFlags();
    setPriority(200); // Default top priority
    setPacketUniverse(1);
    connectUDP();
}

void ofxACNSender::update()
{
	sendDMX();
}

void ofxACNSender::setPacketUniverse(int universe)
{
	// Set header with appropriate universe, high and low byte
	try {
		sac_packet.at(113) = universe >> 8; // Grabs high byte
		sac_packet.at(114) = universe; // Just the low byte
	}
	catch (std::exception& e) {
		cout << "set universe" << e.what() << endl;
	}
}

std::pair<int, int> ofxACNSender::setChannel(int universe, int channel, u_char value)
{
	return setChannels(universe, channel, &value, 1);
}

std::pair<int, int> ofxACNSender::setChannels(int universe, int startChannel, u_char* values, size_t size)
{
	// Expect values between 1 - 512, inclusive
    if ((startChannel < 1) || (startChannel > 512)) {
        ofLog() << "Channel must be between 1 and 512 for DMX protocol. " << startChannel;
        return std::make_pair(0, 0);
	}

    // Check if universe is already in our map
    if (universePackets.count(universe) == 0) {
        std::array<char, 512> emptyPayload = { { char(512) } };

        // Create packet
        UniverseData data = UniverseData{
            char(0),
            emptyPayload
        };
        
        universePackets[universe] = data;
    }

    setPacketUniverse(universe);

	auto& dataPacket = universePackets.at(universe);
    
    int channel = startChannel;
	while (channel < startChannel + size && channel <= 512) {
		dataPacket.payload.at(channel - 1) = (char)*(values + channel - startChannel);
        channel++;
	}

	if ((startChannel + size - 1) > 512) { // Expect data to fit in channel
		ofLog() << "Too much data for channel ... loss of data." << startChannel;
	}

	if (channel > 512) {
	    universe++;
	    channel -= 512;
	}

	return std::make_pair(universe, channel);
}

std::pair<int, int> ofxACNSender::setUniverses(int startUniverse, int startChannel, ofPixels dataIn)
{
	vector<u_char> data;
	int i = 0;
	while (i < dataIn.size()) {
		data.push_back(dataIn[i]);
		i++;
	}

    /* mapping...
	for (int x = 0; x < dataIn.getWidth(); x++) {
		for (int y = 0; y < dataIn.getHeight(); y++) {
			ofColor p = dataIn.getColor(x, y);
			data.push_back(static_cast<unsigned char>(p.r));
			data.push_back(static_cast<unsigned char>(p.g));
			data.push_back(static_cast<unsigned char>(p.b));
		}
	}
    */

    return setUniverses(startUniverse, startChannel, data);
    //return setUniverses(startUniverse, startChannel, dataIn.getData(), dataIn.size());
}

std::pair<int, int> ofxACNSender::setUniverses(int startUniverse, int startChannel, vector<u_char> dataIn)
{
	int endChannel = startChannel;
	vector<u_char> data;
    std::pair<int, int> returnVal;

	for (auto& value : dataIn) {
		data.push_back(value);
		endChannel++;
		if (endChannel == 511) {
            returnVal = setChannels(startUniverse, startChannel, data.data(), data.size());
			startChannel = 1;
			endChannel = 1;
			startUniverse++;
			data.clear();
		}
	}

	// Process the remaining values
	if (data.size() > 0) {
        returnVal = setChannels(startUniverse, startChannel, data.data(), data.size());
	}

    return returnVal;
}

void ofxACNSender::setPriority(int priority)
{
    if ((priority >= 0) && (priority <= 200)) {
        sac_packet.at(108) = char(priority);
    }
    else {
		ofLog() << "Priority must be between 0-200";
    }
}

void ofxACNSender::setLengthFlags()
{
    // 16-bit field with the PDU (Protocol Data Unit) length
    // encoded in the lower 12 bits
    // and 0x7 encoded in the top 4 bits

    // There are 3 PDUs in each packet (Root layer, Framing Layer, and DMP layer)

    // To calculate PDU length for RLP section, subtract last index in DMP layer
    // (637 for full payload) from the index before RLP length flag (15)

    // Set length for
    short val = 0x026e; // Index 637-15 = 622 (0x026e)
    char lowByte = 0xff & val; // Get the lower byte
    char highByte = (0x7 << 4) | (val >> 8); // bit shift so 0x7 is in the top 4 bits

    // Set length for Root Layer (RLP)

    sac_packet.at(16) = highByte;
    sac_packet.at(17) = lowByte;

    val = 0x0258; // Index 637-37 = 600 (0x0258)
    lowByte = 0xff & val; // Get the lower byte
    highByte = (0x7 << 4) | (val >> 8); // bit shift so 0x7 is in the top 4 bits

    // Set length for Framing Layer

    sac_packet.at(38) = highByte;
    sac_packet.at(39) = lowByte; // different length!

    val = 0x020B; // Index 637-114 = 523 (0x020B)
    lowByte = 0xff & val; // Get the lower byte
    highByte = (0x7 << 4) | (val >> 8); // bit shift so 0x7 is in the top 4 bits

    // Set length for DMP Layer
    sac_packet.at(115) = highByte;
    sac_packet.at(116) = lowByte;
}

void ofxACNSender::connectUDP()
{
	delete[] pAddr;
    pAddr = new char[ipAddress.length() + 1];
	strcpy(pAddr, ipAddress.c_str());

    udp.Create();
	udp.SetEnableBroadcast(false);
	udp.SetReuseAddress(true);
	udp.SetNonBlocking(true);
	udp.SetSendBufferSize(4096);
	udp.SetTimeoutSend(1);
    if (bMcast) {
        udp.ConnectMcast(pAddr, destPort);
    } else {
        udp.Connect(pAddr, destPort);
    }
}

void ofxACNSender::sendDMX()
{
    // Send for all universes
    for (auto &pair : universePackets) {
        auto universe = pair.first;
        auto &dataPacket = universePackets.at(universe);
        auto payload = dataPacket.payload;

        setPacketUniverse(universe);

        // TODO: Probably a better way to do this.
        for (int i = 0; i < payload.size(); i++) {
            sac_packet.at(126 + i) = payload.at(i);
        }
        // sts::copy ?
		
        // Handle exceptions
        try {
            sac_packet.at(111) = dataPacket.universeSequenceNum;		
            udp.SendAll(sac_packet.data(), packet_length);
			dataPacket.universeSequenceNum = dataPacket.universeSequenceNum + 1;
        }
        catch (std::exception &e) {
            if (!loggedException) {
                ofLog() << "Could not send sACN data - are you plugged into the device? " << e.what();
                loggedException = true;
            }
        }
    }
}

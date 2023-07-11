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
    int channel = startChannel;
    int packetSize = size;
    int channelOffset = 0;
    while (packetSize > 0)
    {
        createNewUniverse(universe);
        setPacketUniverse(universe);

        auto& dataPacket = universePackets.at(universe);

        while (channel < startChannel + size && channel <= 510) {
            dataPacket.payload.at(channel - 1) = (char)*(values + (channelOffset+channel )- startChannel);
            channel++;
            packetSize--;
        }
        if (packetSize > 0)
        {
            channelOffset = size - packetSize;
            startChannel = 1;
            channel = 1;
            size = packetSize;
            universe++;
        }
    }

    return std::make_pair(universe, channel);
}

std::pair<int, int> ofxACNSender::setPixel(int startUniverse, int startChannel, ofColor col) {
    // RGB
    std::pair<int, int> nextChannel = setChannel(startUniverse, startChannel, col.r);
    nextChannel = setChannel(nextChannel.first, nextChannel.second, col.g);
    return setChannel(nextChannel.first, nextChannel.second, col.b);
}

void ofxACNSender::createNewUniverse(int universe)
{
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
}

std::pair<int, int> ofxACNSender::setChannels(int startUniverse, int startChannel, ofPixels dataIn)
{
    return setChannels(startUniverse, startChannel, dataIn.getData(), dataIn.size());
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
        memcpy(&sac_packet.at(126), payload.data(), payload.size());
 
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

#include <protocol.h>


const uint8_t PACKET_HEADER_START_MARKER = 0xFFU;

const uint8_t HEADER_START_MARKER_OFFSET = 0U;
const uint8_t HEADER_PAYLOAD_CRC_OFFSET = 1U;
const uint8_t HEADER_PAYLOAD_ID_OFFSET = 2U;
const uint8_t HEADER_PAYLOAD_LENGTH_OFFSET = 3U;

extern inline uint8_t calculateCrc(const uint8_t *buffer, uint8_t length)
{
    uint8_t crc = 0U;
    for (uint8_t index = 0U; index < length; ++index)
    {
        crc ^= buffer[index];
    }
    return crc;
}

extern inline uint8_t calculatePacketCrc(const uint8_t *packet)
{
    return calculateCrc(&packet[HEADER_PAYLOAD_ID_OFFSET], getPacketLength(packet) - HEADER_PAYLOAD_ID_OFFSET);
}

const uint8_t packetLengthById[8] = {
    sizeof(PingPacket),
    sizeof(PongPacket),
    sizeof(SensorPacket)};

const uint8_t payloadLengthById[8] = {
    sizeof(PingPayload),
    sizeof(PongPayload),
    sizeof(SensorPayload)};

extern inline uint8_t getPacketLength(const uint8_t *packet)
{
    return packetLengthById[packet[HEADER_PAYLOAD_ID_OFFSET]];
}

extern inline int verifyPacket(const uint8_t *packet)
{
    if (packet[0] != PACKET_HEADER_START_MARKER)
    {
        return 0;
    }
    if (packet[HEADER_PAYLOAD_CRC_OFFSET] != calculatePacketCrc(packet))
    {
        return 0;
    }
    return 1;
}

uint8_t findNextStartMarkerIndex(const uint8_t *buffer, uint8_t bufferSize)
{
    uint8_t startHeaderCandidateIndex = 1U;
    while ((startHeaderCandidateIndex < bufferSize)
           && (buffer[startHeaderCandidateIndex] != PACKET_HEADER_START_MARKER))
    {
        ++startHeaderCandidateIndex;
    }
    return startHeaderCandidateIndex;
}

uint8_t shiftBuffer(uint8_t *buffer, uint8_t shiftedBeginIndex, uint8_t bufferSize)
{
    for (uint8_t index = shiftedBeginIndex; index < bufferSize; ++index)
    {
        buffer[index - shiftedBeginIndex] = buffer[index];
    }
    return bufferSize - shiftedBeginIndex;
}

extern inline void shiftNoise(uint8_t *buffer, uint8_t *bufferIndex)
{
    uint8_t shiftedBeginIndex = findNextStartMarkerIndex(buffer, *bufferIndex);
    *bufferIndex = shiftBuffer(buffer, shiftedBeginIndex, *bufferIndex);
}


extern inline void shiftPacket(uint8_t *buffer, uint8_t *bufferIndex)
{
    uint8_t shiftedBeginIndex = getPacketLength(buffer);
    *bufferIndex = shiftBuffer(buffer, shiftedBeginIndex, *bufferIndex);
}


void processIncomingByte(uint8_t *buffer,
                         uint8_t *bufferIndex,
                         uint8_t bufferSize,
                         uint32_t systemClockMillis,
                         uint8_t incomingByte,
                         processIncomingPacket_t processIncomingPacket)
{
    static const uint32_t nextByteIntervalMillis = 10U;
    static uint32_t nextByteDeadline = 0U;

    if (systemClockMillis > nextByteDeadline)
    {
        *bufferIndex = 0U;
    }

    nextByteDeadline = systemClockMillis + nextByteIntervalMillis;

    buffer[(*bufferIndex)++] = incomingByte;

    if (*bufferIndex >= bufferSize)
    {
        shiftPacket(&buffer[0], bufferIndex);
        return;
    }

    if (*bufferIndex >= sizeof(PacketHeader))
    {
        if (*bufferIndex == getPacketLength(buffer))
        {
            if (verifyPacket(buffer))
            {
                (*processIncomingPacket)(buffer);
                *bufferIndex = 0U;
            }
            else
            {
                shiftNoise(buffer, bufferIndex);
            }
        }
    }
}

void setHeaderFinalizeCrc(uint8_t *buffer)
{
    buffer[HEADER_PAYLOAD_CRC_OFFSET] = calculatePacketCrc(buffer);
}

void setHeaderWithoutCrc(uint8_t *buffer, uint8_t payloadId)
{
    buffer[HEADER_START_MARKER_OFFSET] = PACKET_HEADER_START_MARKER;
    buffer[HEADER_PAYLOAD_ID_OFFSET] = payloadId;
    buffer[HEADER_PAYLOAD_LENGTH_OFFSET] = payloadLengthById[payloadId];
}

void setHeader(uint8_t *buffer, uint8_t payloadId)
{
    setHeaderWithoutCrc(buffer, payloadId);
    setHeaderFinalizeCrc(buffer);
}

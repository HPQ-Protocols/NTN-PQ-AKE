#include "Node.h"
#include <cstring>

Define_Module(Node);

// Gửi gói qua mô hình NTN có packet loss, jitter và trễ hàng đợi.
// Khi cả ba tham số bằng 0, hàm tương đương send() của mô hình gốc.
void Node::sendWithImpairments(AKEMessage *packet)
{
    // Mô phỏng mất gói độc lập.
    if (packetLossRate > 0.0 &&
        uniform(0.0, 1.0) < packetLossRate) {

        packetsDropped++;

        EV << getFullName()
           << ": Mat goi "
           << packet->getName()
           << " theo mo hinh packet loss.\n";

        delete packet;
        return;
    }

    // SatelliteChannel vẫn giữ propagation delay cố định.
    // Hai thành phần dưới đây cộng thêm jitter và queueing delay.
    simtime_t extraDelay = SIMTIME_ZERO;

    if (jitterStddev > SIMTIME_ZERO) {
        double jitterSample =
                normal(0.0, jitterStddev.dbl());

        // Chỉ bổ sung delay không âm.
        if (jitterSample < 0.0)
            jitterSample = -jitterSample;

        extraDelay += SimTime(jitterSample);
    }

    if (congestionDelayMean > SIMTIME_ZERO) {
        extraDelay +=
                SimTime(exponential(
                        congestionDelayMean.dbl()));
    }

    sendDelayed(packet, extraDelay, "port$o");
}

void Node::initialize()
{
    // Tần số CPU dùng để chuyển cycles thành thời gian.
    f_cpu = par("cpuFrequency").doubleValue();

    // Tham số stochastic từ AKENetwork.ned và omnetpp.ini.
    // Giá trị 0 giữ nguyên thí nghiệm deterministic.
    packetLossRate =
            par("packetLossRate").doubleValue();

    jitterStddev =
            SimTime(par("jitterStddev").doubleValue());

    congestionDelayMean =
            SimTime(
                par("congestionDelayMean").doubleValue());

    // Kiểm tra tính hợp lệ của cấu hình.
    if (packetLossRate < 0.0 ||
        packetLossRate > 1.0) {

        throw cRuntimeError(
            "packetLossRate must be in [0,1]");
    }

    if (jitterStddev < SIMTIME_ZERO) {
        throw cRuntimeError(
            "jitterStddev must be non-negative");
    }

    if (congestionDelayMean < SIMTIME_ZERO) {
        throw cRuntimeError(
            "congestionDelayMean must be non-negative");
    }

    // Alice bắt đầu handshake bằng bước xử lý M1.
    if (strcmp("alice", getName()) == 0) {
        cMessage *timer =
                new cMessage("timer_M1");

        int cyclesM1 =
                par("cyclesM1").intValue();

        double cpuDelay =
                static_cast<double>(cyclesM1) /
                f_cpu;

        scheduleAt(
            simTime() + cpuDelay,
            timer);
    }
}

void Node::handleMessage(cMessage *msg)
{
    // =========================================================
    // 1. SELF-MESSAGE: HOÀN TẤT MỘT PHA XỬ LÝ CPU
    // =========================================================
    if (msg->isSelfMessage()) {

        // -----------------------------------------------------
        // Alice hoàn tất xử lý M1
        // -----------------------------------------------------
        if (strcmp("timer_M1",
                   msg->getName()) == 0) {

            AKEMessage *m1 =
                    new AKEMessage("M1_Packet");

            m1->setType(M1_MSG);

            int payload =
                    par("payloadM1").intValue();

            int overhead =
                    par("overheadM1").intValue();

            m1->setPayloadBytes(payload);
            m1->setOverheadBytes(overhead);
            m1->setByteLength(
                    payload + overhead);

            // Thống kê attempted transmission,
            // kể cả khi loss model làm rơi gói.
            totalMsgSent++;
            totalBytesSent += payload + overhead;
            totalOverheadSent += overhead;

            sendWithImpairments(m1);

            EV << "Alice: Da xu ly va thu gui M1. "
               << "[Total Size: "
               << payload + overhead
               << " Bytes]\n";
        }

        // -----------------------------------------------------
        // Bob hoàn tất xử lý M2
        // -----------------------------------------------------
        else if (strcmp("timer_M2",
                        msg->getName()) == 0) {

            AKEMessage *m2 =
                    new AKEMessage("M2_Packet");

            m2->setType(M2_MSG);

            int payload =
                    par("payloadM2").intValue();

            int overhead =
                    par("overheadM2").intValue();

            m2->setPayloadBytes(payload);
            m2->setOverheadBytes(overhead);
            m2->setByteLength(
                    payload + overhead);

            totalMsgSent++;
            totalBytesSent += payload + overhead;
            totalOverheadSent += overhead;

            sendWithImpairments(m2);

            EV << "Bob: Da xu ly va thu gui M2. "
               << "[Total Size: "
               << payload + overhead
               << " Bytes]\n";
        }

        // -----------------------------------------------------
        // Alice hoàn tất xử lý M3
        // -----------------------------------------------------
        else if (strcmp("timer_M3",
                        msg->getName()) == 0) {

            AKEMessage *m3 =
                    new AKEMessage("M3_Packet");

            m3->setType(M3_MSG);

            int payload =
                    par("payloadM3").intValue();

            int overhead =
                    par("overheadM3").intValue();

            m3->setPayloadBytes(payload);
            m3->setOverheadBytes(overhead);
            m3->setByteLength(
                    payload + overhead);

            totalMsgSent++;
            totalBytesSent += payload + overhead;
            totalOverheadSent += overhead;

            sendWithImpairments(m3);

            EV << "Alice: Da xu ly va thu gui M3. "
               << "[Total Size: "
               << payload + overhead
               << " Bytes]\n";
        }

        // -----------------------------------------------------
        // Bob hoàn tất xử lý M4
        // -----------------------------------------------------
        else if (strcmp("timer_M4",
                        msg->getName()) == 0) {

            int payload =
                    par("payloadM4").intValue();

            int overhead =
                    par("overheadM4").intValue();

            if (payload + overhead > 0) {
                // NTN-PQ-AKE: Bob hoàn thành cục bộ
                // và gửi confirmation cuối về Alice.
                handshakeSucceeded = true;
                completionTime = simTime();

                AKEMessage *m4 =
                        new AKEMessage("M4_Packet");

                m4->setType(M4_MSG);
                m4->setPayloadBytes(payload);
                m4->setOverheadBytes(overhead);
                m4->setByteLength(
                        payload + overhead);

                totalMsgSent++;
                totalBytesSent += payload + overhead;
                totalOverheadSent += overhead;

                sendWithImpairments(m4);

                EV << "Bob: Da xu ly va thu gui M4 "
                   << "ve Alice. [Total Size: "
                   << payload + overhead
                   << " Bytes]\n";
            }
            else {
                // Baseline ba flight hoàn tất tại Bob.
                handshakeSucceeded = true;
                completionTime = simTime();

                EV << "Bob: Handshake ba flight "
                   << "da hoan tat.\n";
            }
        }

        delete msg;
        return;
    }

    // =========================================================
    // 2. NETWORK MESSAGE: NHẬN MỘT FLIGHT TỪ KÊNH
    // =========================================================
    AKEMessage *incoming =
            check_and_cast<AKEMessage *>(msg);

    packetsReceived++;

    // ---------------------------------------------------------
    // Bob nhận M1, bắt đầu xử lý M2
    // ---------------------------------------------------------
    if (incoming->getType() == M1_MSG &&
        strcmp("bob", getName()) == 0) {

        cMessage *timer =
                new cMessage("timer_M2");

        int cyclesM2 =
                par("cyclesM2").intValue();

        double cpuDelay =
                static_cast<double>(cyclesM2) /
                f_cpu;

        scheduleAt(
            simTime() + cpuDelay,
            timer);
    }

    // ---------------------------------------------------------
    // Alice nhận M2, bắt đầu xử lý M3
    // ---------------------------------------------------------
    else if (incoming->getType() == M2_MSG &&
             strcmp("alice", getName()) == 0) {

        cMessage *timer =
                new cMessage("timer_M3");

        int cyclesM3 =
                par("cyclesM3").intValue();

        double cpuDelay =
                static_cast<double>(cyclesM3) /
                f_cpu;

        scheduleAt(
            simTime() + cpuDelay,
            timer);
    }

    // ---------------------------------------------------------
    // Bob nhận M3, bắt đầu xử lý M4
    // ---------------------------------------------------------
    else if (incoming->getType() == M3_MSG &&
             strcmp("bob", getName()) == 0) {

        cMessage *timer =
                new cMessage("timer_M4");

        int cyclesM4 =
                par("cyclesM4").intValue();

        double cpuDelay =
                static_cast<double>(cyclesM4) /
                f_cpu;

        scheduleAt(
            simTime() + cpuDelay,
            timer);
    }

    // ---------------------------------------------------------
    // Alice nhận confirmation cuối M4
    // ---------------------------------------------------------
    else if (incoming->getType() == M4_MSG &&
             strcmp("alice", getName()) == 0) {

        // Alice là endpoint xác nhận hoàn tất end-to-end
        // đối với NTN-PQ-AKE bốn flight.
        handshakeSucceeded = true;
        completionTime = simTime();

        EV << "Alice: Da nhan M4; NTN-PQ-AKE "
           << "hoan tat end-to-end.\n";
    }

    delete incoming;
}

void Node::finish()
{
    // Thống kê tại từng endpoint.
    recordScalar(
        "Messages Sent",
        totalMsgSent);

    recordScalar(
        "Total Bytes Sent",
        totalBytesSent);

    recordScalar(
        "Total Overhead Bytes",
        totalOverheadSent);

    recordScalar(
        "Packets Dropped",
        packetsDropped);

    recordScalar(
        "Packets Received",
        packetsReceived);

    recordScalar(
        "Local Handshake Success",
        handshakeSucceeded ? 1 : 0);

    // Chỉ ghi một kết quả end-to-end cho mỗi replication:
    // - NTN-PQ-AKE bốn flight: Alice quan sát hoàn tất.
    // - Baseline ba flight: Bob quan sát hoàn tất.
    bool fourFlightProtocol =
            par("payloadM4").intValue() > 0;

    bool isCompletionObserver =
            fourFlightProtocol
            ? strcmp("alice", getName()) == 0
            : strcmp("bob", getName()) == 0;

    if (isCompletionObserver) {
        recordScalar(
            "End-to-End Handshake Success",
            handshakeSucceeded ? 1 : 0);

        recordScalar(
            "End-to-End Completion Time",
            handshakeSucceeded
                ? completionTime.dbl()
                : -1.0,
            "s");
    }
}
#ifndef __NTN_PQ_AKE_SIM_NODE_H_
#define __NTN_PQ_AKE_SIM_NODE_H_

#include <omnetpp.h>
#include "AKEMessage_m.h"

using namespace omnetpp;

class Node : public cSimpleModule
{
  private:
    // Tần số CPU dùng để chuyển cycles thành thời gian.
    double f_cpu = 0.0;

    // Tham số mô hình mạng stochastic.
    // Giá trị 0 giữ hành vi deterministic ban đầu.
    double packetLossRate = 0.0;
    simtime_t jitterStddev = SIMTIME_ZERO;
    simtime_t congestionDelayMean = SIMTIME_ZERO;

    // Trạng thái hoàn tất handshake.
    bool handshakeSucceeded = false;
    simtime_t completionTime = SIMTIME_ZERO;

    // Thống kê impairment và nhận gói.
    int packetsDropped = 0;
    int packetsReceived = 0;

    // Thống kê chi phí truyền thông.
    // Tính attempted transmissions, kể cả khi loss model
    // làm rơi gói sau khi endpoint thử gửi.
    int totalMsgSent = 0;
    long totalBytesSent = 0;
    long totalOverheadSent = 0;

    // Gửi gói với packet loss, jitter và
    // congestion-delay proxy.
    void sendWithImpairments(AKEMessage *packet);

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

#endif
#ifndef __NTN_PQ_AKE_SIM_NODE_H_
#define __NTN_PQ_AKE_SIM_NODE_H_

#include <omnetpp.h>
#include "AKEMessage_m.h" // Nhớ include file message đã được biên dịch tự động

using namespace omnetpp;

class Node : public cSimpleModule
{
  private:
    double f_cpu;

    // --- CÁC BIẾN THỐNG KÊ CHI PHÍ TRUYỀN THÔNG ---
    int totalMsgSent = 0;         // Đếm số lượng Message đã gửi
    long totalBytesSent = 0;      // Đếm tổng số Byte truyền dẫn thực tế
    long totalOverheadSent = 0;   // Đếm tổng số Byte rác / tiêu đề cấu trúc

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override; // Khai báo thêm hàm finish để xuất dữ liệu ra file .sca
};

#endif

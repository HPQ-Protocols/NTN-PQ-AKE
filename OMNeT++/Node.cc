#include "Node.h"

Define_Module(Node);

void Node::initialize()
{
    // Đọc tần số CPU cấu hình từ file omnetpp.ini (mặc định 168 MHz)
    f_cpu = par("cpuFrequency").doubleValue();

    // Alice là người chủ động khởi tạo giao thức (Giai đoạn M1)
    if (strcmp("alice", getName()) == 0) {
        cMessage *timer = new cMessage("timer_M1");
        // ĐỌC KIỂU INT KHỚP VỚI NED
        int cyclesM1 = par("cyclesM1").intValue();
        // Ép kiểu sang double khi chia để giữ độ chính xác của trễ thời gian
        double delay = (double)cyclesM1 / f_cpu;
        scheduleAt(simTime() + delay, timer);
    }
}

void Node::handleMessage(cMessage *msg)
{
    // TRƯỜNG HỢP 1: Hết thời gian xử lý của CPU (Self-message mô phỏng trễ tính toán)
    if (msg->isSelfMessage()) {
        if (strcmp("timer_M1", msg->getName()) == 0) {
            // Alice xử lý xong M1 -> Đóng gói gửi cho Bob
            AKEMessage *m1 = new AKEMessage("M1_Packet");
            m1->setType(M1_MSG);

            int payload = par("payloadM1").intValue();
            int overhead = par("overheadM1").intValue();

            m1->setPayloadBytes(payload);
            m1->setOverheadBytes(overhead);
            m1->setByteLength(payload + overhead);

            send(m1, "port$o");

            // Cộng dồn thống kê chi phí
            totalMsgSent++;
            totalBytesSent += (payload + overhead);
            totalOverheadSent += overhead;

            EV << "Alice: Tinh toan xong M1. Da gui M1 sang Bob. [Total Size: " << (payload+overhead) << " Bytes]\n";
        }
        else if (strcmp("timer_M2", msg->getName()) == 0) {
            // Bob xử lý xong M2 -> Đóng gói gửi cho Alice
            AKEMessage *m2 = new AKEMessage("M2_Packet");
            m2->setType(M2_MSG);

            int payload = par("payloadM2").intValue();
            int overhead = par("overheadM2").intValue();

            m2->setPayloadBytes(payload);
            m2->setOverheadBytes(overhead);
            m2->setByteLength(payload + overhead);

            send(m2, "port$o");

            totalMsgSent++;
            totalBytesSent += (payload + overhead);
            totalOverheadSent += overhead;

            EV << "Bob: Tinh toan xong M2. Da gui M2 sang Alice. [Total Size: " << (payload+overhead) << " Bytes]\n";
        }
        else if (strcmp("timer_M3", msg->getName()) == 0) {
            // Alice xử lý xong M3 -> Đóng gói gửi lại Bob
            AKEMessage *m3 = new AKEMessage("M3_Packet");
            m3->setType(M3_MSG);

            int payload = par("payloadM3").intValue();
            int overhead = par("overheadM3").intValue();

            m3->setPayloadBytes(payload);
            m3->setOverheadBytes(overhead);
            m3->setByteLength(payload + overhead);

            send(m3, "port$o");

            totalMsgSent++;
            totalBytesSent += (payload + overhead);
            totalOverheadSent += overhead;

            EV << "Alice: Tinh toan xong M3. Hoan thanh thiet lap khoa! [Total Size: " << (payload+overhead) << " Bytes]\n";
        }
        else if (strcmp("timer_M4", msg->getName()) == 0) {
            // Bob xử lý xong bước cuối cùng -> Thỏa thuận khóa thành công
            EV << "Bob: Tinh toan xong M4. Giao thuc hoan thanh vinh vien!\n";
        }
        delete msg;
    }
    // TRƯỜNG HỢP 2: Nhận được gói tin thực tế từ đường truyền mạng
    else {
        AKEMessage *incoming = check_and_cast<AKEMessage *>(msg);

        if (incoming->getType() == M1_MSG && strcmp("bob", getName()) == 0) {
            // Bob nhận M1 từ Alice -> Bắt đầu tính toán M2 bằng CPU của mình
            cMessage *timer = new cMessage("timer_M2");
            int cyclesM2 = par("cyclesM2").intValue(); // ĐỌC KIỂU INT KHỚP VỚI NED
            double delay = (double)cyclesM2 / f_cpu;
            scheduleAt(simTime() + delay, timer);
        }
        else if (incoming->getType() == M2_MSG && strcmp("alice", getName()) == 0) {
            // Alice nhận M2 từ Bob -> Bắt đầu tính toán M3
            cMessage *timer = new cMessage("timer_M3");
            int cyclesM3 = par("cyclesM3").intValue(); // ĐỌC KIỂU INT KHỚP VỚI NED
            double delay = (double)cyclesM3 / f_cpu;
            scheduleAt(simTime() + delay, timer);
        }
        else if (incoming->getType() == M3_MSG && strcmp("bob", getName()) == 0) {
            // Bob nhận M3 từ Alice -> Thực hiện nốt bước cuối M4
            cMessage *timer = new cMessage("timer_M4");
            int cyclesM4 = par("cyclesM4").intValue(); // ĐỌC KIỂU INT KHỚP VỚI NED
            double delay = (double)cyclesM4 / f_cpu;
            scheduleAt(simTime() + delay, timer);
        }
        delete incoming;
    }
}

void Node::finish()
{
    recordScalar("Messages Sent", totalMsgSent);
    recordScalar("Total Bytes Sent", totalBytesSent);
    recordScalar("Total Overhead Bytes", totalOverheadSent);
}

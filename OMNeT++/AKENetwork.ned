// 1. Phần định nghĩa module Node (đã làm ở bước trước)
simple Node {
    parameters:
        double cpuFrequency @unit(Hz) = default(168000000Hz);
        int cyclesM1 = default(0);
        int cyclesM2 = default(0);
        int cyclesM3 = default(0);
        int cyclesM4 = default(0);
        int payloadM1 = default(0);
        int payloadM2 = default(0);
        int payloadM3 = default(0);
        int overheadM1 = default(0);
        int overheadM2 = default(0);
        int overheadM3 = default(0);
    gates:
        inout port; // Đảm bảo cổng inout trùng với port$o trong Node.cc
}

// 2. PHẦN BẮT BUỘC PHẢI CÓ: Định nghĩa mạng chạy mô phỏng
network AKENetwork
{
    @display("bgb=400,200");
    types:
        channel SatelliteChannel extends ned.DatarateChannel {
            delay = 125ms;       // Trễ truyền dẫn vệ tinh (Ví dụ GEO là 125ms một chiều)
            datarate = 10Mbps;   // Băng thông kênh truyền vệ tinh
        }
    submodules:
        alice: Node {
            @display("p=100,100");
        }
        bob: Node {
            @display("p=300,100");
        }
    connections:
        alice.port <--> SatelliteChannel <--> bob.port;
}
// =============================================================
// 1. SIMPLE MODULE: NODE
// =============================================================
simple Node
{
    parameters:
        // Tần số CPU dùng để chuyển cycles thành processing delay.
        double cpuFrequency @unit(Hz) = default(168000000Hz);

        // Cycle counts của bốn pha xử lý.
        int cyclesM1 = default(0);
        int cyclesM2 = default(0);
        int cyclesM3 = default(0);
        int cyclesM4 = default(0);

        // Payload và protocol overhead của từng flight.
        int payloadM1 = default(0);
        int payloadM2 = default(0);
        int payloadM3 = default(0);
        int payloadM4 = default(0);

        int overheadM1 = default(0);
        int overheadM2 = default(0);
        int overheadM3 = default(0);
        int overheadM4 = default(0);

        // Tham số stochastic network impairment.
        // Giá trị 0 giữ nguyên mô hình deterministic.
        
        // Xác suất làm mất độc lập từng packet, miền [0,1].
        double packetLossRate = default(0);

        // Độ lệch chuẩn của additional one-way delay.
        double jitterStddev @unit(s) = default(0s);

        // Trung bình của exponential queueing-delay proxy.
        double congestionDelayMean @unit(s) = default(0s);

    gates:
        inout port;
}

// =============================================================
// 2. NETWORK: ALICE -- SATELLITE CHANNEL -- BOB
// =============================================================
network AKENetwork
{
    @display("bgb=400,200");

    types:
        channel SatelliteChannel extends ned.DatarateChannel
        {
            // Propagation delay của deterministic baseline.
            delay = 125ms;

            // Datarate của kênh NTN.
            datarate = 10Mbps;
        }

    submodules:
        alice: Node
        {
            @display("p=100,100");
        }

        bob: Node
        {
            @display("p=300,100");
        }

    connections:
        alice.port <--> SatelliteChannel <--> bob.port;
}
import matplotlib.pyplot as plt
import numpy as np

# Cài đặt font chuẩn bài báo khoa học
plt.rcParams.update({'font.size': 11, 'font.family': 'serif'})

# ==========================================
# 1. DỮ LIỆU ĐO ĐẠC
# ==========================================
protocols = ['NTN-PQ-AKE\n(Proposed)', 'TLS 1.3 PSK\n+ Kyber', 'KEMTLS\n(No Sig)', 
             'PQ-TLS\n(NIST)', 'Classical\nTLS', 'Hybrid\nTLS']

# Data Phần cứng
cycles = [3956170, 3435009, 4598640, 10824936, 66046764, 68771860]
ram_kb = [(124+68656)/1024, (168+73624)/1024, (124+78128)/1024, (156+79760)/1024, (156+69080)/1024, (156+73816)/1024]
rom_kb = [48900/1024, 52036/1024, 48204/1024, 67284/1024, 38664/1024, 63544/1024]

# Data Mô phỏng Mạng
bytes_sent = [2406, 2660, 4990, 6524, 1696, 3712]
latency_ms = [400.5, 397.6, 406.4, 444.6, 769.5, 787.3]

x = np.arange(len(protocols))

# ==========================================
# FIGURE 1: COMBO CHART (CYCLES & MEMORY)
# ==========================================
fig1, ax1 = plt.subplots(figsize=(10, 6))

# Trục Y Trái (CPU Cycles - Cột)
color_cycles = '#2b83ba'
ax1.set_xlabel('Protocols', fontweight='bold')
ax1.set_ylabel('CPU Cycles (Log Scale)', color=color_cycles, fontweight='bold')
bars = ax1.bar(x, cycles, color=color_cycles, alpha=0.8, width=0.5, edgecolor='black', label='CPU Cycles')
ax1.tick_params(axis='y', labelcolor=color_cycles)
ax1.set_yscale('log')
ax1.set_xticks(x)
ax1.set_xticklabels(protocols)
ax1.set_ylim(10**6, 10**8)

# Trục Y Phải (Memory - Đường)
ax2 = ax1.twinx()  
color_ram = '#d7191c'
color_rom = '#fdae61'
ax2.set_ylabel('Memory Footprint (KB)', color='black', fontweight='bold')
line1 = ax2.plot(x, ram_kb, color=color_ram, marker='o', markersize=8, linewidth=2.5, label='RAM (Stack Memory)')
line2 = ax2.plot(x, rom_kb, color=color_rom, marker='s', markersize=8, linewidth=2.5, label='Flash (Code Size)')
ax2.tick_params(axis='y', labelcolor='black')
ax2.set_ylim(30, 85)

# Ghi chú trực tiếp số RAM/ROM lên điểm Line
for i, txt in enumerate(ram_kb):
    ax2.annotate(f"{txt:.1f}", (x[i], ram_kb[i]), textcoords="offset points", xytext=(0,10), ha='center', fontsize=9, color=color_ram)
for i, txt in enumerate(rom_kb):
    ax2.annotate(f"{txt:.1f}", (x[i], rom_kb[i]), textcoords="offset points", xytext=(0,-15), ha='center', fontsize=9, color='#e66100')

# Hợp nhất Chú giải (Legend)
lines_labels = [bars] + line1 + line2
labels = [l.get_label() for l in lines_labels]
ax1.legend(lines_labels, labels, loc='upper left', framealpha=0.9)

plt.title('Hardware Evaluation on STM32F407G (Cycles vs. Memory)', fontweight='bold', pad=15)
plt.grid(axis='y', linestyle='--', alpha=0.3)
plt.tight_layout()
plt.savefig('Fig1_Hardware_Combo.png', dpi=300)


# ==========================================
# FIGURE 2: SCATTER PLOT (TRADE-OFF MATRIX)
# ==========================================
fig2, ax3 = plt.subplots(figsize=(9, 6.5))

# Các màu sắc đặc trưng cho điểm
colors = ['#762a83', '#1b7837', '#e7298a', '#d95f02', '#7570b3', '#e6ab02']
markers = ['*', 'o', 's', 'D', '^', 'v']

# Vẽ từng điểm lên đồ thị phân tán
for i in range(len(protocols)):
    # Phóng to và làm nổi bật ngôi sao của giao thức đề xuất
    size = 400 if i == 0 else 150 
    edge = 'black' if i == 0 else 'gray'
    ax3.scatter(bytes_sent[i], latency_ms[i], s=size, c=colors[i], marker=markers[i], edgecolors=edge, zorder=3, label=protocols[i].replace('\n', ' '))

# Vẽ vùng "Lý tưởng" (Bottom-Left Corner)
ax3.axhspan(380, 420, xmin=0, xmax=0.35, color='lightgreen', alpha=0.2, zorder=1)
ax3.text(1800, 385, 'Ideal Zone\n(Low Latency, Low Bytes)', color='green', fontsize=10, fontstyle='italic', fontweight='bold')

# Tùy chỉnh trục và nhãn
ax3.set_xlabel('Communication Cost (Total Bytes Transmitted)', fontweight='bold')
ax3.set_ylabel('End-to-End Latency (ms)', fontweight='bold')
ax3.set_title('Network Performance Trade-off (Latency vs. Payload)', fontweight='bold', pad=15)
ax3.grid(True, linestyle='--', alpha=0.6, zorder=0)

# Giới hạn trục để đồ thị đẹp hơn
ax3.set_xlim(1200, 7000)
ax3.set_ylim(350, 850)

# Chú giải
ax3.legend(loc='upper center', bbox_to_anchor=(0.5, -0.15), ncol=3, frameon=True, fontsize=10)

# Ghi chú tên giao thức thẳng lên các điểm
for i, txt in enumerate(protocols):
    clean_txt = txt.replace('\n', ' ')
    if i == 0: # Đề xuất
        ax3.annotate(clean_txt, (bytes_sent[i], latency_ms[i]), xytext=(-30, 20), textcoords='offset points', fontweight='bold', color=colors[i])
    elif i == 4: # Classical
        ax3.annotate(clean_txt, (bytes_sent[i], latency_ms[i]), xytext=(-35, -20), textcoords='offset points')
    else:
        ax3.annotate(clean_txt, (bytes_sent[i], latency_ms[i]), xytext=(15, 0), textcoords='offset points', va='center')

plt.tight_layout()
plt.savefig('Fig2_Network_Tradeoff.png', dpi=300, bbox_inches='tight')
plt.show()
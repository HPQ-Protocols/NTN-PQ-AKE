import matplotlib.pyplot as plt
import numpy as np

# Cài đặt font chuẩn bài báo khoa học IEEE
plt.rcParams.update({'font.size': 11, 'font.family': 'serif'})

# Dữ liệu mô phỏng mạng OMNeT++
protocols = ['NTN-PQ-AKE\n(Proposed)', 'TLS 1.3 PSK\n+ Kyber', 'KEMTLS\n(No Sig)', 
             'PQ-TLS\n(NIST)', 'Classical\nTLS', 'Hybrid\nTLS']

bytes_sent = [2406, 2660, 4990, 6524, 1696, 3712]
latency_ms = [400.5, 397.6, 406.4, 444.6, 769.5, 787.3]

fig2, ax3 = plt.subplots(figsize=(9, 6.5))

# Các màu sắc đặc trưng cho điểm dữ liệu
colors = ['#762a83', '#1b7837', '#e7298a', '#d95f02', '#7570b3', '#e6ab02']
markers = ['*', 'o', 's', 'D', '^', 'v']

# Vẽ các điểm phân tán
for i in range(len(protocols)):
    size = 400 if i == 0 else 150 
    edge = 'black' if i == 0 else 'gray'
    ax3.scatter(bytes_sent[i], latency_ms[i], s=size, c=colors[i], marker=markers[i], 
                edgecolors=edge, zorder=3, label=protocols[i].replace('\n', ' '))

# =========================================================================
# SỬA LỖI ĐÈ CHỮ: SỬ DỤNG TỌA ĐỘ TRỤC TƯƠNG ĐỐI (AXES FRACTION)
# =========================================================================
# 1. Mũi tên chỉ hướng giảm chi phí truyền thông (←) - Nằm sát đáy đồ thị
ax3.annotate('', xy=(0.04, 0.04), xytext=(0.28, 0.04), xycoords='axes fraction',
             arrowprops=dict(arrowstyle="->", color='dimgray', lw=1.5))
ax3.text(0.16, 0.06, 'Lower Communication Cost', transform=ax3.transAxes,
         color='dimgray', fontsize=9.5, ha='center', va='bottom')

# 2. Mũi tên chỉ hướng giảm thời gian bắt tay (↓) - Nằm sát rìa trái bên trong
ax3.annotate('', xy=(0.03, 0.08), xytext=(0.03, 0.32), xycoords='axes fraction',
             arrowprops=dict(arrowstyle="->", color='dimgray', lw=1.5))
ax3.text(0.05, 0.20, 'Lower Handshake Time', transform=ax3.transAxes,
         color='dimgray', fontsize=9.5, ha='left', va='center', rotation=90)
# =========================================================================

# Tùy chỉnh trục và nhãn học thuật
ax3.set_xlabel('Communication Cost (Total Bytes Transmitted)', fontweight='bold')
ax3.set_ylabel('End-to-End Latency (ms)', fontweight='bold')
ax3.set_title('Network Performance Trade-off (Latency vs. Payload)', fontweight='bold', pad=15)
ax3.grid(True, linestyle='--', alpha=0.5, zorder=0)

# Giới hạn trục hợp lý để tạo khoảng trống an toàn cho mũi tên ở góc dưới-trái
ax3.set_xlim(1200, 7000)
ax3.set_ylim(330, 850)

# Chú giải (Legend) đặt gọn gàng phía dưới đồ thị
ax3.legend(loc='upper center', bbox_to_anchor=(0.5, -0.15), ncol=3, frameon=True, fontsize=10)

# Ghi chú nhãn tên giao thức trực tiếp cạnh các điểm dữ liệu
for i, txt in enumerate(protocols):
    clean_txt = txt.replace('\n', ' ')
    if i == 0: # Giao thức đề xuất
        ax3.annotate(clean_txt, (bytes_sent[i], latency_ms[i]), xytext=(-35, 18), textcoords='offset points', fontweight='bold', color=colors[i])
    elif i == 4: # Classical TLS (Đẩy nhãn sang phải một chút để né hoàn toàn mũi tên trục Y)
        ax3.annotate(clean_txt, (bytes_sent[i], latency_ms[i]), xytext=(15, -10), textcoords='offset points')
    else:
        ax3.annotate(clean_txt, (bytes_sent[i], latency_ms[i]), xytext=(15, 0), textcoords='offset points', va='center')

plt.tight_layout()
plt.savefig('Fig2_Network_Tradeoff_IEEE_Fixed.png', dpi=300, bbox_inches='tight')
plt.show()
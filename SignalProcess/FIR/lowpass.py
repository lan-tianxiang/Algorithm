import numpy as np
import matplotlib.pyplot as plt

# 定义滤波器参数
N = 100  # 滤波器阶数
fc = 0.2  # 截止频率
fs = 1  # 采样频率

# 计算归一化截止频率
Wn = fc / (fs/2)

# 计算汉宁窗函数
win = np.ones(N)
for n in range(1, N):
    win[n] *= (N - n + 1) / N

# 计算频率响应函数（即理想低通滤波器的频率响应）
H = 2 * np.sinc(Wn * np.arange(N))

# 计算滤波器系数
b = H * win

# 生成输入信号和噪声信号
t = np.arange(0, N) / fs
x = np.sin(2 * np.pi * 0.1 * t) + np.sin(2 * np.pi * 0.2 * t) + np.sin(2 * np.pi * 0.3 * t)
n = np.random.randn(*x.shape)

# 对输入信号进行滤波处理
y = np.convolve(x, b, mode='same')

# 绘制结果图像
plt.figure()
plt.subplot(3, 1, 1)
plt.plot(t, x)
plt.title('Input Signal')
plt.xlabel('Time (s)')
plt.ylabel('Amplitude')
plt.subplot(3, 1, 2)
plt.plot(t, n)
plt.title('Noise Signal')
plt.xlabel('Time (s)')
plt.ylabel('Amplitude')
plt.subplot(3, 1, 3)
plt.plot(t, y)
plt.title('Output Signal')
plt.xlabel('Time (s)')
plt.ylabel('Amplitude')
plt.tight_layout()
plt.show()

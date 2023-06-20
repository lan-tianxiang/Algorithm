import numpy as np
import resampy
from scipy.signal import resample_poly
# Plot the original and resampled signals
import matplotlib.pyplot as plt

# 原始采样率
original_sample_rate = 10  # 假设原始采样率为1000 Hz

# 目标采样率
target_sample_rate = 20  # 假设目标采样率为2000 Hz

# 原始信号
original_signal = np.sin(2 * np.pi * 10 * np.linspace(0, 1, original_sample_rate))

# 计算采样率比率（重采样系数）
resample_ratio = target_sample_rate / original_sample_rate

# 多相重采样
resampled_signal = resampy.resample(original_signal, original_sample_rate, target_sample_rate)

# 多项式重采样
resampled_signal2 = resample_poly(original_signal, up=resample_ratio, down=1)

# 打印结果
print("原始信号：", original_signal)
print("重采样后的信号：", resampled_signal)
print("重采样后的信号：", resampled_signal2)

plt.plot(original_signal, label='Original signal')
plt.plot(resampled_signal, label='Resampled signal')
plt.plot(resampled_signal2, label='Resampled signal2')
plt.legend()
plt.show()
import numpy as np
from scipy.signal import impulse, firwin, lfilter, unit_impulse

# 定义混响系统的单位脉冲响应
impulse_response = [0.2, 0.9, 0.3, -0.1]

# 生成单位脉冲信号
impulse_signal = unit_impulse(len(impulse_response))

# 使用FIR滤波器拟合混响系统的单位脉冲响应
order = len(impulse_response) - 1  # FIR滤波器阶数为单位脉冲响应的长度减1
fir_coeff = firwin(order+1, cutoff=0.5)  # 生成FIR滤波器系数
filtered_signal = lfilter(fir_coeff, [1.0], impulse_signal)  # 应用滤波器

# 打印拟合结果
print("拟合结果:", filtered_signal)

# 打印实际单位脉冲响应
print("实际单位脉冲响应:", impulse_response)

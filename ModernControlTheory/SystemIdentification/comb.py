import numpy as np
from scipy import signal
from scipy.optimize import minimize
from scipy.signal import impulse, lfilter, unit_impulse

# 定义混响系统的单位脉冲响应
impulse_response = [0.2, 0.5, 0.3, -0.1, 0.2, 0.1, 0.05]

# 生成单位脉冲信号
impulse_signal = unit_impulse(len(impulse_response))

# 定义拟合目标函数
def objective(target,coeff):
    filtered_signal = lfilter(coeff, [1.0], impulse_response)
    error = np.sum((filtered_signal - impulse_response) ** 2)
    return error

# 初始化滤波器系数的初始猜测值
initial_guess = np.zeros(len(impulse_response))  # 将滤波器系数定义为一个一维数组

# 使用优化算法进行拟合
result = minimize(objective, initial_guess, args=(impulse_response), method='BFGS')

# 提取拟合得到的滤波器系数
fitted_coeff = result.x

# 应用拟合得到的滤波器系数进行信号处理
filtered_signal = lfilter(fitted_coeff, [1.0], impulse_signal)

# 打印拟合结果
print("拟合结果:", filtered_signal)
#print("拟合系数:", fitted_coeff)

# 打印实际单位脉冲响应
print("实际单位脉冲响应:", impulse_response)

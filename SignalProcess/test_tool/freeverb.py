import numpy as np
from scipy.signal import lfilter

def comb_filter(input_signal, delay, decay):
    output_signal = np.zeros_like(input_signal)
    delay_buffer = np.zeros(delay)
    
    for i in range(len(input_signal)):
        # 读取延迟线上的输出
        delayed_sample = delay_buffer[i % delay]
        
        # 混合输入信号和延迟线输出
        mixed_signal = input_signal[i] + decay * delayed_sample
        
        # 更新延迟线
        delay_buffer = np.roll(delay_buffer, 1)
        delay_buffer[0] = mixed_signal
        
        # 将混合后的信号叠加到输出
        output_signal[i] += mixed_signal
    
    return output_signal


def allpass_filter(input_signal, delay, decay):
    output_signal = np.zeros_like(input_signal)
    delay_buffer = np.zeros(delay)
    
    for i in range(len(input_signal)):
        # 读取延迟线上的输出
        delayed_sample = delay_buffer[i % delay]
        
        # 混合输入信号和延迟线输出
        mixed_signal = input_signal[i] + decay * delayed_sample
        
        # 更新延迟线
        delay_buffer = np.roll(delay_buffer, 1)
        delay_buffer[0] = mixed_signal
        
        # 应用全通滤波器
        allpass_filtered = mixed_signal - decay * delayed_sample
        
        # 将全通滤波器输出叠加到输出
        output_signal[i] += allpass_filtered
    
    return output_signal


def freeverb(input_signal, wet_gain, wet_mix, dry_mix, room_size, dampening):
    output_signal = np.zeros_like(input_signal)
    input_length = len(input_signal)
    
    # 计算延迟线的延迟时间和衰减因子
    delay_times = [int(room_size) for _ in range(4)]
    decay_factors = [dampening for _ in range(4)]
    
    # 预设全通滤波器的反馈系数
    allpass_feedback = 0.5
    
    # 叠加混响效果和干净信号
    for i in range(input_length):
        wet_signal = np.zeros_like(input_signal)
        
        # 应用4个并行的组合滤波器
        for j in range(4):
            wet_signal += comb_filter(input_signal, delay_times[j], decay_factors[j])
        
        # 应用2个串行的全通滤波器
        wet_signal = allpass_filter(wet_signal, int(room_size * 0.5), allpass_feedback)
        wet_signal = allpass_filter(wet_signal, int(room_size * 0.3), allpass_feedback)
        
        # 控制混响效果和干净信号的混合比例
        output_signal[i] = (wet_gain * wet_signal + wet_mix * input_signal[i]) / (wet_gain + wet_mix) * wet_mix + input_signal[i] * dry_mix
    
    return output_signal

import numpy as np
from scipy.signal import lfilter
from pydub import AudioSegment
import dask.array as da

def comb_filter(input_signal, delay, decay):
    output_signal = da.zeros(len(input_signal))
    delay_buffer = da.zeros(delay)
    
    for i in range(len(input_signal)):
        # 读取延迟线上的输出
        delayed_sample = delay_buffer[i % delay]
        
        # 混合输入信号和延迟线输出
        mixed_signal = input_signal[i] + decay * delayed_sample
        
        # 更新延迟线
        delay_buffer = da.roll(delay_buffer, 1)
        delay_buffer[0] = mixed_signal
        
        # 将混合后的信号叠加到输出
        output_signal[i] += mixed_signal
    
    return output_signal


def allpass_filter(input_signal, delay, decay):
    output_signal = da.zeros(len(input_signal))
    delay_buffer = da.zeros(delay)
    
    for i in range(len(input_signal)):
        # 读取延迟线上的输出
        delayed_sample = delay_buffer[i % delay]
        
        # 混合输入信号和延迟线输出
        mixed_signal = input_signal[i] + decay * delayed_sample
        
        # 更新延迟线
        delay_buffer = da.roll(delay_buffer, 1)
        delay_buffer[0] = mixed_signal
        
        # 应用全通滤波器
        allpass_filtered = mixed_signal - decay * delayed_sample
        
        # 将全通滤波器输出叠加到输出
        output_signal[i] += allpass_filtered
    
    return output_signal


def freeverb(input_signal, wet_gain, wet_mix, dry_mix, room_size, dampening):
    output_signal = da.zeros(len(input_signal))
    input_length = len(input_signal)
    
    # 计算延迟线的延迟时间和衰减因子
    delay_times = [int(room_size) for _ in range(4)]
    decay_factors = [dampening for _ in range(4)]
    
    # 预设全通滤波器的反馈系数
    allpass_feedback = 0.5
    
    # 叠加混响效果和干净信号
    for i in range(input_length):
        wet_signal = da.zeros(len(input_signal))
        
        # 应用4个并行的组合滤波器
        for j in range(4):
            wet_signal += comb_filter(input_signal, delay_times[j], decay_factors[j])
        
        # 应用2个串行的全通滤波器
        wet_signal = allpass_filter(wet_signal, int(room_size * 0.5), allpass_feedback)
        wet_signal = allpass_filter(wet_signal, int(room_size * 0.3), allpass_feedback)
        
        # 控制混响效果和干净信号的混合比例
        output_signal[i] = (wet_gain * wet_signal + wet_mix * input_signal[i]) / (wet_gain + wet_mix) * wet_mix + input_signal[i] * dry_mix
    
    return output_signal

def read_audio_file(filename):
    audio = AudioSegment.from_file(filename)
    return audio

def write_audio_file(filename, audio_data):
    audio = AudioSegment(audio_data.tobytes(), frame_rate=44100, sample_width=2, channels=1)
    audio.export(filename, format='wav')

def main():
    # 读取音频文件
    input_signal = read_audio_file('/home/highdimen/Downloads/Code/Algorithm/SignalProcess/test_tool/test.wav')
    
    # 设置混响参数
    wet_gain = 0.5
    wet_mix = 0.5
    dry_mix = 0.5
    room_size = 5000
    dampening = 0.5
    
    # 应用混响效果
    output_signal = freeverb(input_signal, wet_gain, wet_mix, dry_mix, room_size, dampening)
    
    # 将混响效果输出到音频文件
    write_audio_file('output.wav', output_signal)

main()
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import butter, lfilter

# Bandpass filter
def butter_bandpass(lowcut, highcut, fs, order=5):
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = butter(order, [low, high], btype='band')
    return b, a

def butter_bandpass_filter(data, lowcut, highcut, fs, order=5):
    b, a = butter_bandpass(lowcut, highcut, fs, order=order)
    y = lfilter(b, a, data)
    return y

# Lowpass filter
def butter_lowpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype='low', analog=False)
    return b, a

def butter_lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = lfilter(b, a, data)
    return y



def filter_boundaries(freqval, lower_bpm, higher_bpm):
    lower_limit = float(lower_bpm)/60 
    upper_limit = float(higher_bpm)/60
    start = -1
    for i in xrange(len(freqval)):
        if freqval[i] >= lower_limit:
            start = i
            break
    end = -1
    for i in xrange(len(freqval)):
        if freqval[i] >= upper_limit:
            end = i
            break

    if start == -1 or end == -1:
        return None # insufficient freq values
    return start, end

def find_peak_within_range(fftval, start, end):
    peak = 0.0
    peak_freq = -1
    for i in xrange(start, end):
        if fftval[i-1]<fftval[i]>fftval[i+1] and fftval[i] > peak:
            peak = fftval[i]
            peak_freq = i
    return peak_freq


def cal_heart_rate(fftval, freqval):
    start, end = filter_boundaries(freqval, 40.0, 220.0)
    peak_freq = find_peak_within_range(fftval, start, end)
    return freqval[peak_freq]

def cal_resp_rate(fftval, freqval):
    start, end = filter_boundaries(freqval, 8, 22)
    peak_freq = find_peak_within_range(fftval, start, end)
    return freqval[peak_freq]


def findMaxMins(arr):
    maximas = list()
    minimas = list()
    for i in xrange(1, len(arr)-1):
        if arr[i-1]<arr[i]>arr[i+1]:
            maximas.append(i)
        if arr[i-1]>arr[i]<arr[i+1]:
            minimas.append(i)
    return maximas, minimas

        


# Read data from CSV
data = np.genfromtxt("/Users/bhushan/Downloads/takashin_Homework_sample.csv",delimiter=",", skip_header=1, dtype=float)
data_mod = np.empty((data.shape[0], 3))
data_mod[:,:] = data[:,0:3]

########### Heart rate and respiration rate calculation ##########
signal = data_mod[:,2]
sample_freq = 50
timestep = 1.0/sample_freq
num_splits = 1
HEART_RATE = 0.0
RESP_RATE = 0.0

for subarr in np.array_split(signal, num_splits):
    avg = np.mean(subarr)
    subarr = subarr-avg
    fourier = np.fft.fft(subarr)
    abs_val = abs(fourier)**2
    freq = np.fft.fftfreq(subarr.size, d=timestep)
    
    heart_rate_freq = cal_heart_rate(abs_val, freq)
    filtered_signal = butter_bandpass_filter(subarr, heart_rate_freq-0.5, heart_rate_freq+0.5, sample_freq)    
    maxs, mins = findMaxMins(filtered_signal[1000:])
    heartrate_arr = [(60*50/float(maxs[i]-maxs[i-1])) for i in range(1, len(maxs))]
    HEART_RATE += np.mean(heartrate_arr)/num_splits
    print "Heart rate:- "
    print heartrate_arr

    resp_rate_freq = cal_resp_rate(abs_val, freq)
    filtered_signal = butter_bandpass_filter(subarr, 0.2, 0.4, sample_freq, order = 4)    
    maxs, mins = findMaxMins(filtered_signal[1000:])
    resprate_arr = [(60*50/float(maxs[i]-maxs[i-1])) for i in range(1, len(maxs))]
    resprate_arr = filter(lambda x: 6<x<24,  resprate_arr)
    RESP_RATE += np.mean(resprate_arr)/num_splits
    print "Respiration rate:- "
    print resprate_arr

print "Final Heart Rate:- ", HEART_RATE
print "Final Respiration Rate:- ", RESP_RATE


######## SPO2 Calculation ########
IR = data[:, 2]
RED = data[:, 1]
# Filtering IR and RED signal to remove Noise
IR = butter_lowpass_filter(IR, 2, sample_freq, 6)[100:]
RED = butter_lowpass_filter(RED, 2, sample_freq, 6)[100:]
# Finding minimas and maximas
xvals = [_ for _ in xrange(len(IR))]
red_maxs, red_mins = findMaxMins(RED)
ir_maxs, ir_mins = findMaxMins(IR)
# Calculating interpolation for minimas in RED signal
red_yvals = [RED[i] for i in red_mins]
red_yinterp = np.interp(xvals, red_mins, red_yvals)
# Calculating interpolation for minimas in IR signal
ir_yvals = [IR[i] for i in ir_mins]
ir_yinterp = np.interp(xvals, ir_mins, ir_yvals)

# Calculating AC and DC component in IR and RED signal
AC_IR = [(IR[i] - ir_yinterp[i]) for i in ir_maxs]
DC_IR = [ir_yinterp[i] for i in ir_maxs]
AC_RED = [(RED[i] - red_yinterp[i]) for i in ir_maxs]
DC_RED = [red_yinterp[i] for i in ir_maxs]

# Calculating SpO2
spo2ans = []    
for i in range(len(AC_RED)):
    rat = (AC_RED[i]*DC_IR[i])/(AC_IR[i]*DC_RED[i])
    spo2_itr = -45.060* rat * rat + 30.354 * rat + 94.845
    spo2ans.append(spo2_itr)
print "SpO2 values:- "
print spo2ans
print "Final SpO2:- ", np.mean(spo2ans)


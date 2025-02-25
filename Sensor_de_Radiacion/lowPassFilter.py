# https://stackoverflow.com/questions/25191620/
#   creating-lowpass-filter-in-scipy-understanding-methods-and-units

import numpy as np
from scipy.signal import butter, filtfilt, freqz, hilbert
from matplotlib import pyplot as plt


def butter_lowpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype='low', analog=False)
    return b, a

def offsetRemover(data):
    offset = float((np.max(data)+np.min(data))/2)
    return [val - offset for val in data]
    # return data

def butter_lowpass_filter_full_wave(data, cutoff, fs, order=5):
    data = [(-1)*val if val < 0 else val for val in data]
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = filtfilt(b, a, data)
    # y=data
    return y

def pasa_bandas(senal, frecuencia_corte, cantidad_muestras, delta_tiempo):
    "Esta función se encarga de eliminar las frecuencias altas y la parte DC de la señal deseada"
    if cantidad_muestras > 0:
        delta_frecuencia = 1 / (delta_tiempo*cantidad_muestras)   #Se calcula los delta frecuencia
        x_fourier = np.linspace(0, (len(senal)) /(2*delta_frecuencia), len(senal)) #Se calculan las frecuencias en el espacio de Fourier, este array contiene los índices frecuenciales
        distancia = x_fourier
        filtro = np.where(distancia <= 15, 0, 1 ) * np.where(distancia <= frecuencia_corte, 1,0)
        senal_fourier = np.fft.fft(senal)
        senal_fourierfiltro = senal_fourier * filtro
        senal_demodulada = np.fft.ifft(senal_fourierfiltro)
        senal_demodulada = np.abs(senal_demodulada)
        senal_demodulada -= (np.max(senal_demodulada)/2)
    return senal_demodulada
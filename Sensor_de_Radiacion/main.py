import serial
import time
import threading
import numpy as np
import tkinter as tk
from collections import deque
import matplotlib.pyplot as plt
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import lowPassFilter as lp

# Configuración del puerto serial
arduino = serial.Serial("COM6", 9600)
time.sleep(2)

# Parámetros globales
delta_tiempo = 0.01
buffer_size = 1000
ventana_fft = 1000 #Número de muestras
delta_frecuencia = 1 / (ventana_fft * delta_tiempo)

# Buffers de datos
x_Generada = deque(maxlen=buffer_size)
x_Recibida = deque(maxlen=buffer_size)
tiempo = deque(maxlen=buffer_size)
RMS = deque(maxlen=200)
x_FourierRecibida = np.zeros(ventana_fft // 2)

# Marca de tiempo inicial
tiempo_inicio = time.time()

# Interfaz gráfica con Tkinter
root = tk.Tk()
root.title("Monitor de Señales AM")
root.geometry("1200x800")

# Label para mostrar el valor RMS
rms_label = tk.Label(root, text="Voltaje RMS: --", font=("Arial", 14))
rms_label.pack()

# Crear una figura con 4 subgráficos (2x2)
fig, axs = plt.subplots(2, 2, figsize=(10, 8), dpi=100)
(ax1, ax2), (ax3, ax4) = axs  # Desempaquetar los ejes

# Títulos y configuraciones
titles = ["Señal AM Generada", "Señal AM Recibida", "Transformada de Fourier", "RMS Señal Recibida"]
colors = ['red', 'orange', 'blue', 'gray']

for ax, title, color in zip([ax1, ax2, ax3, ax4], titles, colors):
    ax.set_title(title)
    ax.grid(True)
    ax.set_xlabel("Tiempo (s)" if ax != ax3 else "Frecuencia (Hz)")
    ax.set_ylabel("Amplitud")
    ax.set_facecolor("#f0f0f0")

# Integrar en Tkinter
canvas = FigureCanvasTkAgg(fig, master=root)
canvas.get_tk_widget().pack()

# Función de actualización de gráficos
def actualizar_graficas():
    global tiempos_relativos
    ax1.clear()
    ax2.clear()
    ax3.clear()
    ax4.clear()
    
    if tiempo:
        tiempos_relativos = np.array(tiempo) - tiempo[0]
        min_len = min(len(tiempos_relativos), len(x_Generada), len(x_Recibida))
        tiempos_relativos = tiempos_relativos[:min_len]
        x_Generada_arr = np.array(x_Generada)[:min_len]
        x_Recibida_arr = np.array(x_Recibida)[:min_len]
        
        ax1.plot(tiempos_relativos, x_Generada_arr, color='red')
        ax1.set_xlim(21, 24)
        if 400 == len(demodulacion):
            ax2.plot(tiempos_relativos[-400:], demodulacion, color ='red' )
        ax2.set_xlim (18, 24)    
        ax2.set_ylim (-0.3, 0.3)
        
        frecuencias = np.linspace(0, (len(x_FourierRecibida)) /(2*delta_frecuencia), len(x_FourierRecibida))
        ax3.set_ylim(0, 100)
        ax3.set_xlim(0, 2000)
        ax3.plot(frecuencias, np.abs(x_FourierRecibida), color='blue')

        if len(RMS) == 200:
            ax4.plot(tiempos_relativos[-200:], RMS, color='gray')
            ax4.set_ylim(0,0.4)
            ax4.set_xlim(21,24)
        
    for ax, title in zip([ax1, ax2, ax3, ax4], titles):
        ax.set_title(title)
        ax.grid(True)
    
    canvas.draw()
    root.after(100, actualizar_graficas)

# Hilo de procesamiento de FFT
def procesar_fft():
    global x_FourierRecibida
    while True:
        time.sleep(0.5)
        if len(x_Recibida) >= ventana_fft:
            datos_fft = np.array(x_Recibida)[-ventana_fft:]
            transformada = np.fft.fft(datos_fft)
            x_FourierRecibida = np.abs(transformada)
            
def procesar_RMS(senal, frecuencia_Corte):
    '''Esta función demodula y devuelve el valor RMS de la señal demodulada y un array que contiene la señal demodulada'''
    global RMS
    senal_demodulada = lp.pasa_bandas(senal[-400:], frecuencia_Corte, len(senal[-100:]), 0.1)
    senal_ultimos100 = senal_demodulada[-50:]
    valor_RMSinstantaneo =np.sqrt(np.mean(np.square(senal_ultimos100)))
    RMS.append(valor_RMSinstantaneo)
    return RMS , senal_demodulada

# Variable global para almacenar el valor RMS
rms_valor = 0.0

# Hilo de procesamiento de RMS
def procesar_RMS_thread():
    global demodulacion
    global rms_valor
    demodulacion = []
    while True:
        time.sleep(0.01)
        nuevo_rms, demodulacion = procesar_RMS(list(x_Recibida), 500)
        rms_valor = nuevo_rms[-1]  # Tomamos el último valor calculado

def actualizar_rms_label():
    """Actualiza el label de voltaje RMS en la interfaz gráfica"""
    rms_label.config(text=f"Voltaje RMS: {rms_valor:.3f} V")  # Formato con 3 decimales
    root.after(100, actualizar_rms_label)  # Repetir cada 100 ms

# Iniciar la actualización del label
root.after(100, actualizar_rms_label)

# Hilo de simulación de datos
def main_Cycle():
    global x_Recibida
    while True:
        try:
            datos_Arduino = arduino.readline().decode("utf-8", errors="ignore").strip()
            if datos_Arduino:
                valores = datos_Arduino.split(",")
                if len(valores) < 2:
                    continue  # Evita procesar datos incompletos
                try:
                    array_Datos = list(map(float, valores))
                except ValueError:
                    print(f"Error de conversión: {datos_Arduino}")  # Muestra el problema en consola
                    continue  # Salta esta iteración si hay caracteres no válidos

                tiempo.append(time.time() - tiempo_inicio)
                x_Generada.append(array_Datos[0])
                x_Recibida.append(array_Datos[1])

        except KeyboardInterrupt:
            print("Interrumpido por el usuario")
            break
    root.quit()

# Iniciar hilos
t_main = threading.Thread(target=main_Cycle, daemon=True)
t_fft = threading.Thread(target=procesar_fft, daemon=True)
t_rms = threading.Thread(target=procesar_RMS_thread, daemon=True)

t_main.start()
t_fft.start()
t_rms.start()

# Iniciar la actualización de gráficos
root.after(100, actualizar_graficas)
root.mainloop()
arduino.close()
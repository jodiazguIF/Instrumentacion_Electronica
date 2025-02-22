import serial
import time
import threading
import numpy as np
import tkinter as tk
from collections import deque
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

#Configuración del puerto serial
arduino = serial.Serial("COM6", 9600)
time.sleep(2)

# Parámetros globales
delta_tiempo = 1 / 2000
buffer_size = 2000  # Máximo de datos en las listas
ventana_fft = 2000  # Tamaño de la ventana de la FFT
delta_frecuencia = 1 / (ventana_fft * delta_tiempo)  # Resolución espectral

# Buffers de datos
x_Generada = deque(maxlen=buffer_size)
x_Recibida = deque(maxlen=buffer_size)
tiempo = deque(maxlen=buffer_size)
x_FourierRecibida = np.zeros(ventana_fft // 2)  # FFT solo en la mitad positiva

# Marca de tiempo inicial
tiempo_inicio = time.time()

# Interfaz gráfica con Tkinter
root = tk.Tk()
root.title("Monitor de Señales AM")
root.geometry("1200x800")

# Crear una figura con 3 subgráficos
fig = Figure(figsize=(10, 8), dpi=100)
ax1 = fig.add_subplot(311)  # Señal AM Generada
ax2 = fig.add_subplot(312)  # Señal AM Recibida
ax3 = fig.add_subplot(313)  # FFT de la señal AM Recibida

titles = ["Señal AM Generada", "Señal AM Recibida", "Transformada de Fourier de la Señal AM Recibida"]
axes = [ax1, ax2, ax3]
colors = ['red', 'orange', 'blue']

for ax, title, color in zip(axes, titles, colors):
    ax.set_title(title)
    ax.grid(True)
    ax.set_xlabel("Tiempo (s)" if ax != ax3 else "Frecuencia (Hz)")
    ax.set_ylabel("Amplitud")

canvas = FigureCanvasTkAgg(fig, master=root)
canvas.get_tk_widget().pack()

# Función de actualización de gráficos
def actualizar_graficas():
    ax1.clear()
    ax2.clear()
    ax3.clear()

    if tiempo:
        tiempos_relativos = np.array(tiempo) - tiempo[0]

        # Asegurar que tiempo tenga la misma longitud que los datos
        min_len = min(len(tiempos_relativos), len(x_Generada), len(x_Recibida))
        tiempos_relativos = tiempos_relativos[:min_len]
        x_Generada_arr = np.array(x_Generada)[:min_len]
        x_Recibida_arr = np.array(x_Recibida)[:min_len]

        ax1.plot(tiempos_relativos, x_Generada_arr, color='red', label=titles[0])
        ax2.plot(tiempos_relativos, x_Recibida_arr, color='orange', label=titles[1])

        # Ajuste en la FFT
        frecuencias = np.linspace(0, (len(x_FourierRecibida) - 1) * delta_frecuencia, len(x_FourierRecibida))
        ax3.plot(frecuencias, np.abs(x_FourierRecibida), color='blue', label=titles[2])

    for ax, title in zip(axes, titles):
        ax.set_title(title)
        ax.grid(True)
        ax.legend()

    canvas.draw()
    root.after(100, actualizar_graficas)

# Hilo de procesamiento de FFT
def procesar_fft():
    global x_FourierRecibida
    while True:
        time.sleep(0.5)
        if len(x_Recibida) >= ventana_fft:
            datos_fft = np.array(x_Recibida)[-ventana_fft:]  # Últimos datos
            transformada = np.fft.fftshift(np.fft.fft(datos_fft))  # FFT
            x_FourierRecibida = np.abs(transformada[:ventana_fft // 2])  # Solo parte positiva

# Hilo de simulación de datos
def main_Cycle():
    global x_Recibida
    while True:
        try:
            datos_Arduino = arduino.readline().decode().strip()
            if datos_Arduino:
                array_Datos = list(map(float, datos_Arduino.split(",")))
                if len(array_Datos) < 2:
                    continue

                tiempo.append(time.time() - tiempo_inicio)
                x_Generada.append(array_Datos[0])
                x_Recibida.append(array_Datos[1])

        except KeyboardInterrupt:
            print("Interrumpido por el usuario")
            break
    arduino.close()
    root.quit()

# Iniciar hilos
t_main = threading.Thread(target=main_Cycle, daemon=True)
t_fft = threading.Thread(target=procesar_fft, daemon=True)

t_main.start()
t_fft.start()

# Iniciar la actualización de gráficos
root.after(100, actualizar_graficas)
root.mainloop()

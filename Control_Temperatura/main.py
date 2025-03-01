import serial
import time
import threading
import tkinter as tk

# Configura el puerto serial (ajusta 'COMx' al puerto en uso)
arduino = serial.Serial('COM5', 9600)  # Cambiar el primer argumento según configuración 
time.sleep(2)
entrada = 25
temperatura_actual = 25

def actualizar_entrada():
    '''Con esta función se actualiza la entrada para el Arduino, en este caso el set_temperature''' 
    global entrada
    try:
        entrada = float(entrada_entry.get())  # Captura el valor del campo de texto
        print(f"Temperatura deseada: {entrada} °C")
    except ValueError:
        time.sleep(0.01)

def input_usuario():
    '''Esta función sirve para que el usuario establezca la entrada que desea a través
    de la interfaz gráfica'''
    while True:
        try:
            entrada = float(entrada_entry.get())
        except ValueError:
            entrada = temperatura_actual 

# Crear la ventana de Tkinter
root = tk.Tk()
root.title("Control de Temperatura")
root.geometry("700x600")

# Etiqueta y campo de entrada para la altura deseada
label_entrada = tk.Label(root, text="Temperatura deseada en °C:")
label_entrada.pack(pady=10)

entrada_entry = tk.Entry(root)
entrada_entry.insert(0, float(25))  # Inserta el valor por defecto
entrada_entry.pack(pady=10)

boton_actualizar = tk.Button(root, text="Actualizar Temperatura", command=actualizar_entrada)
boton_actualizar.pack(pady=10)    

# Lienzo para la gráfica
canvas = tk.Canvas(root, width=600, height=300, bg="white")
canvas.pack(pady=20)

# Caja para mostrar la temperatura actual
label_temp_actual = tk.Label(root, text="Temperatura Actual: 0 °C", font=("Arial", 16), fg="red")
label_temp_actual.pack(pady=10)

# Variables para la gráfica
x_pos = 50
last_temp = 150

# Dibujar ejes
canvas.create_line(40, 10, 40, 200, fill="black", width=2)  # Eje Y
canvas.create_line(40, 200, 600, 200, fill="black", width=2)  # Eje X

# Etiquetas de temperatura en el eje Y
for i in range(0, 110, 10):
    y_pos = 200 - (i * 1.5)
    canvas.create_text(25, y_pos, text=str(i), fill="black", font=("Arial", 8))

# Función para actualizar la gráfica
def graficar_temp():
    global x_pos, last_temp, temperatura_actual
    try:
        temp_actual = float(temperatura_actual)
    except ValueError:
        temp_actual = last_temp

    # Actualizar la etiqueta de temperatura actual
    label_temp_actual.config(text=f"Temperatura Actual: {temp_actual:.1f} °C")

    # Dibujar línea en la gráfica
    y_pos = 200 - (temp_actual * 1.5)  # Escalar temperatura a la altura del canvas
    if 0 <= y_pos <= 200:
        canvas.create_line(x_pos, last_temp, x_pos + 5, y_pos, fill="red", width=2)
        last_temp = y_pos
        x_pos += 5

    # Limpiar la gráfica si se sale del ancho del canvas
    if x_pos > 600:
        canvas.delete("all")
        # Redibujar los ejes
        canvas.create_line(40, 10, 40, 200, fill="black", width=2)  # Eje Y
        canvas.create_line(40, 200, 600, 200, fill="black", width=2)  # Eje X
        for i in range(0, 110, 10):
            y_pos = 200 - (i * 1.5)
            canvas.create_text(25, y_pos, text=str(i), fill="black", font=("Arial", 8))
        x_pos = 50

    root.after(500, graficar_temp)  # Llamar a la función cada 500 ms

# Inicia el hilo para el input del usuario
input_thread = threading.Thread(target=input_usuario, daemon=True)    
input_thread.start()    
    
def main_cycle():
    global temperatura_actual, entrada
    while True:
        try:
            temperatura_actual = arduino.readline().decode('utf-8').strip()  # Lee el valor desde Arduino, este ya es el valor de temperatura actual
            #print(temperatura_actual)
            arduino.write(f"{entrada}\n".encode())  #Se envía el dato de temperatura deseado a Python
        except KeyboardInterrupt:
            print("Interrumpido por el usuario")
            break
           
# Se inicia el ciclo principal en un hilo separado
ciclo_thread = threading.Thread(target=main_cycle, daemon=True)
ciclo_thread.start()

# Inicia la gráfica en tiempo real
graficar_temp()

# Se ejecuta la interfaz de Tkinter
root.mainloop()

# Esta sección cierra el puerto serial y la interfaz    
arduino.close()  # Cierra el puerto serial una vez terminado
print("Puerto serial cerrado. Saliendo...")
root.quit()  # Cierra la interfaz gráfica de Tkinter

import serial
import time
import threading
import tkinter as tk

# Configura el puerto serial (ajusta 'COMx' al puerto en uso)
arduino = serial.Serial('COM5', 9600)  # Cambiar el primer argumento según configuración 
time.sleep(2)
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
root.geometry("400x500")

# Etiqueta y campo de entrada para la altura deseada
label_entrada = tk.Label(root, text="Temperatura deseada en °C:")
label_entrada.pack(pady=10)

entrada_entry = tk.Entry(root)
entrada_entry.insert(0, float(22))  # Inserta el valor por defecto
entrada_entry.pack(pady=10)

boton_actualizar = tk.Button(root, text="Actualizar Temperatura", command=actualizar_entrada)
boton_actualizar.pack(pady=10)    
    
#Se inicia el hilo para el input del usuario
input_thread = threading.Thread(target=input_usuario, daemon = True)    
input_thread.start()    
    
def main_cycle():
    global temperatura_actual
    while True:
        try:
            temperatura_actual = arduino.readline().decode('utf-8').strip()  # Lee el valor desde Arduino, este ya es el valor de temperatura actual
            arduino.write(f"{entrada}\n".encode())  #Se envía el dato de temperatura deseado a Python
        except KeyboardInterrupt:
            print("Interrumpido por el usuario")
            break
           
#Se inicia el ciclo principal en un hilo separado
ciclo_thread = threading.Thread(target=main_cycle, daemon = True)
ciclo_thread.start()           
           
#Se ejecuta la interfaz de Tkinter
root.mainloop()

#Esta sección cierra el puerto serial y la interfaz    
comando = "0"
arduino.write(comando.encode())
arduino.close()  # Cierra el puerto serial una vez terminado
print("Puerto serial cerrado. Saliendo...")
root.quit()  # Cierra la interfaz gráfica de Tkinter

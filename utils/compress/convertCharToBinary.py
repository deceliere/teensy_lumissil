import os
import struct

# Fonction pour convertir une image en binaire
def convert_image_to_binary(image):
    binary_data = bytearray()
    for val in image:
        binary_data.append(val)  # Ajouter la valeur du pixel (0-255) comme un octet
    return binary_data

# Fonction pour écrire le fichier en binaire
def write_binary_file(binary_data, output_filename):
    with open(output_filename, 'wb') as f:
        f.write(binary_data)

# Charger le fichier source et convertir les pixels en binaire
def convert_text_to_binary(input_filename, output_filename):
    binary_data = bytearray()

    with open(input_filename, 'r') as f:
        for line in f:
            data = list(map(int, line.strip().split(',')))  # Convertir chaque ligne en liste de nombres
            binary_line = convert_image_to_binary(data)     # Convertir la ligne en binaire
            binary_data.extend(binary_line)

    write_binary_file(binary_data, output_filename)

# Mesurer la taille d'un fichier
def get_file_size(filename):
    return os.path.getsize(filename)

# Chemins des fichiers
input_file = 'test_processing_255.txt'  # Ton fichier source avec les valeurs de pixels en texte
output_file = 'image_data_binary.bin'

# Convertir le fichier texte en binaire
convert_text_to_binary(input_file, output_file)

# Afficher les tailles des fichiers
original_size = get_file_size(input_file)
binary_size = get_file_size(output_file)

print(f"Taille du fichier original : {original_size / 1024:.2f} KB")
print(f"Taille du fichier binaire : {binary_size / 1024:.2f} KB")

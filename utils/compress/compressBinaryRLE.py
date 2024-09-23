import os
import struct

# Fonction pour compresser une liste de valeurs (0-255) avec RLE
def rle_compress(data):
    compressed = []
    count = 1
    prev = data[0]

    for i in range(1, len(data)):
        if data[i] == prev and count < 255:  # Limiter à 255 répétitions (1 octet)
            count += 1
        else:
            compressed.append((prev, count))
            prev = data[i]
            count = 1
    compressed.append((prev, count))  # Ajouter le dernier groupe
    return compressed

# Fonction pour encoder une image en binaire et appliquer RLE
def encode_image_as_binary(image):
    # Compresser l'image avec RLE
    compressed_image = rle_compress(image)

    # Stocker en binaire (valeur, répétition)
    binary_data = bytearray()
    for val, count in compressed_image:
        binary_data.append(val)   # Valeur du pixel (0-255)
        binary_data.append(count) # Nombre de répétitions (1-255)

    return binary_data

# Fonction pour calculer la différence entre deux images (P-frame) avec normalisation
def delta_encode(image, prev_image):
    delta_image = []
    for i in range(len(image)):
        delta = image[i] - prev_image[i]
        # Normaliser les différences à la plage 0-255 en ajoutant un décalage de 128
        normalized_delta = delta + 128
        delta_image.append(normalized_delta)
    return delta_image

# Fonction pour écrire le fichier compressé en binaire
def write_binary_file(compressed_data, output_filename):
    with open(output_filename, 'wb') as f:
        for binary_chunk in compressed_data:
            f.write(binary_chunk)

# Charger le fichier source, compresser et écrire le fichier compressé avec I-frames et P-frames
def compress_mpeg_like_binary(input_filename, output_filename, frame_interval):
    compressed_data = []
    prev_image = None
    frame_count = 0

    with open(input_filename, 'r') as f:
        for line in f:
            data = list(map(int, line.strip().split(',')))  # Convertir chaque ligne en liste de nombres

            # Chaque n-ième image sera une I-frame (image complète)
            if frame_count % frame_interval == 0:
                binary_data = encode_image_as_binary(data)  # I-frame en binaire
                prev_image = data  # Mettre à jour l'image de référence
            else:
                # Calcul des différences avec l'image précédente (P-frame)
                delta_image = delta_encode(data, prev_image)
                binary_data = encode_image_as_binary(delta_image)  # P-frame en binaire

            compressed_data.append(binary_data)
            prev_image = data  # Mettre à jour l'image précédente
            frame_count += 1

    write_binary_file(compressed_data, output_filename)

# Mesurer la taille d'un fichier
def get_file_size(filename):
    return os.path.getsize(filename)

# Chemins des fichiers
input_file = 'test_processing_255.txt'  # Ton fichier source
output_file = 'mpeg_like_compressed_binary_image_data.bin'
frame_interval = 10  # Intervalle pour les I-frames (toutes les 10 images)

# Compresser le fichier
compress_mpeg_like_binary(input_file, output_file, frame_interval)

# Afficher les tailles des fichiers
original_size = get_file_size(input_file)
compressed_size = get_file_size(output_file)

print(f"Taille du fichier original : {original_size / 1024:.2f} KB")
print(f"Taille du fichier compressé : {compressed_size / 1024:.2f} KB")

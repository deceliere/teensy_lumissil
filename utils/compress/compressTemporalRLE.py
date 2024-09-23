import os

# Fonction pour compresser une liste de valeurs (0-255) avec RLE
def rle_compress(data):
    compressed = []
    count = 1
    prev = data[0]

    # Parcourir les données et compresser
    for i in range(1, len(data)):
        if data[i] == prev:
            count += 1
        else:
            compressed.append((prev, count))
            prev = data[i]
            count = 1
    compressed.append((prev, count))  # Ajouter le dernier groupe

    return compressed

# Fonction pour calculer la différence entre deux images
def delta_encode(image, prev_image):
    return [image[i] - prev_image[i] for i in range(len(image))]

# Fonction pour écrire le fichier compressé en format (valeur, répétition)
def write_compressed_file(compressed_data, output_filename):
    with open(output_filename, 'w') as f:
        for line in compressed_data:
            f.write(','.join([f"{val}x{count}" for val, count in line]) + '\n')

# Charger le fichier source, compresser et écrire le fichier compressé
def compress_temporal_file(input_filename, output_filename):
    compressed_data = []
    prev_image = None

    with open(input_filename, 'r') as f:
        for line in f:
            data = list(map(int, line.strip().split(',')))  # Convertir chaque ligne en liste de nombres

            if prev_image is None:
                # Première image, on la stocke telle quelle
                compressed_line = rle_compress(data)
            else:
                # Calcul des différences avec l'image précédente
                delta_image = delta_encode(data, prev_image)
                compressed_line = rle_compress(delta_image)

            compressed_data.append(compressed_line)
            prev_image = data  # Mettre à jour l'image précédente

    write_compressed_file(compressed_data, output_filename)

# Mesurer la taille d'un fichier
def get_file_size(filename):
    return os.path.getsize(filename)

# Chemins des fichiers
input_file = 'test_processing_255.txt'  # Ton fichier source
output_file = 'temporal_compressed_image_data.txt'

# Compresser le fichier
compress_temporal_file(input_file, output_file)

# Afficher les tailles des fichiers
original_size = get_file_size(input_file)
compressed_size = get_file_size(output_file)

print(f"Taille du fichier original : {original_size / 1024:.2f} KB")
print(f"Taille du fichier compressé : {compressed_size / 1024:.2f} KB")

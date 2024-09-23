import os

# Fonction pour lire le fichier binaire et reconvertir les octets en valeurs de pixels
def read_binary_file(binary_filename):
    with open(binary_filename, 'rb') as f:
        binary_data = f.read()  # Lire le fichier binaire
        pixels = list(binary_data)  # Convertir les octets en une liste de valeurs (0-255)
    return pixels

# Fonction pour lire le fichier texte et obtenir les valeurs de pixels
def read_text_file(text_filename):
    pixels = []
    with open(text_filename, 'r') as f:
        for line in f:
            data = list(map(int, line.strip().split(',')))  # Convertir chaque ligne en liste de nombres
            pixels.extend(data)  # Ajouter les pixels à la liste
    return pixels

# Fonction pour comparer deux listes de pixels
def compare_pixels(text_pixels, binary_pixels):
    if len(text_pixels) != len(binary_pixels):
        print(f"Erreur : les fichiers ne contiennent pas le même nombre de pixels ({len(text_pixels)} vs {len(binary_pixels)})")
        return False

    for i in range(len(text_pixels)):
        if text_pixels[i] != binary_pixels[i]:
            print(f"Erreur à l'index {i} : Pixel texte = {text_pixels[i]}, Pixel binaire = {binary_pixels[i]}")
            return False

    print("Le fichier binaire correspond exactement au fichier texte.")
    return True

# Chemins des fichiers
input_file = 'test_processing_255.txt'  # Ton fichier source
output_file = 'image_data_binary.bin'  # Ton fichier binaire

# Lire les pixels du fichier texte et du fichier binaire
text_pixels = read_text_file(input_file)
binary_pixels = read_binary_file(output_file)

# Comparer les pixels
is_correct = compare_pixels(text_pixels, binary_pixels)

# Afficher les résultats de la comparaison
if is_correct:
    print("Les deux fichiers sont identiques.")
else:
    print("Les fichiers sont différents.")

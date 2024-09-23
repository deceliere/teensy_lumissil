#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

// Fonction pour normaliser les valeurs entre 0 et 255
std::vector<int> normalize(const std::vector<double>& values, double minVal, double maxVal) {
    std::vector<int> normalizedValues;
    for (double val : values) {
        // Normalisation linéaire entre 0 et 255
        int normalized = static_cast<int>((val - minVal) / (maxVal - minVal) * 255);
        normalizedValues.push_back(normalized);
    }
    return normalizedValues;
}

int main() {
    std::ifstream inFile("source.txt");  // Fichier source
    std::ofstream outFile("mapped.txt"); // Fichier de sortie

    if (!inFile.is_open() || !outFile.is_open()) {
        std::cerr << "Erreur lors de l'ouverture des fichiers !" << std::endl;
        return 1;
    }

    std::vector<double> allValues; // Contient toutes les valeurs du fichier
    std::string line;

    // Première passe : lire toutes les valeurs du fichier
    while (std::getline(inFile, line)) {
        std::stringstream ss(line);
        std::string value;
        while (std::getline(ss, value, ',')) {
            allValues.push_back(std::stod(value));
        }
    }

    // Trouver les valeurs minimale et maximale dans tout le fichier
    double minVal = *std::min_element(allValues.begin(), allValues.end());
    double maxVal = *std::max_element(allValues.begin(), allValues.end());

    // Normaliser toutes les valeurs
    std::vector<int> normalizedValues = normalize(allValues, minVal, maxVal);

    // Remettre les valeurs normalisées dans le fichier de sortie, en conservant la structure du fichier d'origine
    inFile.clear();  // Réinitialiser l'état du fichier
    inFile.seekg(0); // Revenir au début du fichier

    size_t idx = 0;
    while (std::getline(inFile, line)) {
        std::stringstream ss(line);
        std::string value;

        bool first = true;
        while (std::getline(ss, value, ',')) {
            if (!first) {
                outFile << ", "; // Ajouter une virgule entre les valeurs sur la même ligne
            }
            outFile << normalizedValues[idx++];
            first = false;
        }
        outFile << "\n"; // Nouvelle ligne après chaque ligne normalisée
    }

    inFile.close();
    outFile.close();

    std::cout << "Normalisation terminée avec succès." << std::endl;
    return 0;
}

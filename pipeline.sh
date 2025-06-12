#!/bin/bash

# ========== CONFIGURATION ==========
GENETIC_EXEC="./genetic/genetic"
MATRIX_GEN_SCRIPT="matriceDistance.py"
PDF_SCRIPT="createPDF.py"
VENV_DIR="venv"
PYTHON_EXEC="$VENV_DIR/bin/python"
PIP_EXEC="$VENV_DIR/bin/pip"
# ===================================

# Vérification du nombre d'arguments
if [ $# -ne 1 ]; then
    echo "Usage: $0 <chemin_du_fichier_CSV>"
    exit 1
fi

# === 1. Définition des chemins et fichiers ===
CSV_PATH="$1"
CSV_FILENAME=$(basename "$CSV_PATH")
FOLDER_NAME="${CSV_FILENAME%.*}"
PROJECT_ROOT=$(dirname "$(realpath "$0")")
OUTPUT_FOLDER="$PROJECT_ROOT/$FOLDER_NAME"
OUTPUT_FILE="$OUTPUT_FOLDER/output.txt"

echo "Fichier fourni : $CSV_PATH"
echo "Dossier de sortie : $OUTPUT_FOLDER"

# === 2. Création de l'environnement virtuel si nécessaire ===
if [ ! -d "$VENV_DIR" ]; then
    echo "Création d'un environnement virtuel Python..."
    python3 -m venv "$VENV_DIR"
    if [ $? -ne 0 ]; then
        echo "Erreur : échec de création de l'environnement virtuel."
        exit 1
    fi
fi

# === 3. Activation de l'environnement virtuel ===
echo "Activation de l'environnement virtuel..."
source "$VENV_DIR/bin/activate"
$VENV_DIR/bin/pip freeze > requirements.txt

# === 4. Installation des dépendances Python ===
echo "Installation des dépendances Python..."
$PIP_EXEC install --upgrade pip --break-system-packages > /dev/null 2>&1
$PIP_EXEC install --break-system-packages requests numpy geopandas matplotlib pandas contextily fpdf2 > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "Erreur : installation des dépendances échouée."
    deactivate
    exit 1
fi

# === 5. Génération de la matrice de distance ===
echo "Génération de la matrice depuis le fichier CSV..."
$PYTHON_EXEC "$MATRIX_GEN_SCRIPT" "$CSV_PATH"
if [ $? -ne 0 ]; then
    echo "Erreur lors de la génération de la matrice."
    deactivate
    exit 2
fi

# === 6. Compilation du code C ===
echo "Compilation du code C..."
make -C genetic > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "Erreur de compilation du code C."
    deactivate
    exit 3
fi

# === 7. Exécution de l'algorithme génétique ===
echo "Exécution de l'algorithme génétique..."
$GENETIC_EXEC 30 "$OUTPUT_FOLDER" "$OUTPUT_FILE"
if [ $? -ne 0 ]; then
    echo "Erreur lors de l'exécution de l'algorithme génétique."
    deactivate
    exit 4
fi

# === 8. Génération du PDF avec le script Python ===
echo "Génération du PDF à partir des résultats..."
cd "$PROJECT_ROOT" || exit 5
$PYTHON_EXEC "$PDF_SCRIPT" "$OUTPUT_FOLDER" "$OUTPUT_FILE"
if [ $? -ne 0 ]; then
    echo "Erreur lors de la génération du PDF."
    deactivate
    exit 6
fi

# === 9. Désactivation de l'environnement virtuel ===
deactivate

echo "Pipeline terminé avec succès. Résultat disponible dans : $OUTPUT_FILE et PDF généré."

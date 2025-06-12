import argparse
import requests
import numpy as np
import time
import csv
import os

ORS_API_KEY = "5b3ce3597851110001cf62481f7fe86c591e473c9e5474ca97c2263a"

# === 1. Argument : fichier CSV à traiter ===
parser = argparse.ArgumentParser(description="Génère une matrice de distances et durées à partir d'un CSV d'adresses.")
parser.add_argument("csv_file", help="Chemin vers le fichier CSV d'entrée")
args = parser.parse_args()
input_csv_path = args.csv_file

# === 1.1 Vérification et insertion éventuelle de l'adresse de dépôt en première ligne ===
entrepot_ligne = ["Départ entrepôt Cerp", "600 Rue des Madeleines", "77100", "Mareuil-lès-Meaux"]
entrepot_str = ",".join(entrepot_ligne)

with open(input_csv_path, newline='') as f:
    lines = f.readlines()

if not lines or "600 Rue des Madeleines" not in lines[0]:
    lines.insert(0, entrepot_str + "\n")
    with open(input_csv_path, "w", newline='') as f:
        f.writelines(lines)

# === 2. Création du dossier de sortie à la racine du projet ===
base_filename = os.path.basename(input_csv_path)
folder_name = os.path.splitext(base_filename)[0]
project_root = os.path.abspath(os.path.dirname(__file__))
output_dir = os.path.join(project_root, folder_name)
os.makedirs(output_dir, exist_ok=True)

# === 3. Lecture et géocodage avec Nominatim ===
coords = []
noms = []

with open(input_csv_path, newline='') as csvfile:
    for line in csvfile:
        line = line.strip()
        row = line.split(',')
        nom = row[0]
        adresse = row[1]
        postal = row[2]
        ville = row[3]

        adresse_complete = f"{adresse}, {postal} {ville}"
        url = "https://nominatim.openstreetmap.org/search"
        params = {"q": adresse_complete, "format": "json", "limit": 1}
        headers = {"User-Agent": "test/1.0 (email@mail.com)"}
        response = requests.get(url, headers=headers, params=params)
        time.sleep(1)

        data = response.json()
        if data:
            lon = float(data[0]['lon'])
            lat = float(data[0]['lat'])
            coords.append([lon, lat])
            noms.append((nom, adresse, lon, lat))
        else:
            adresseVille = f"{ville}, {postal}"
            params = {"q": adresseVille, "format": "json", "limit": 1}
            response = requests.get(url, headers=headers, params=params)
            time.sleep(1)

            data = response.json()
            if data:
                lon = float(data[0]['lon'])
                lat = float(data[0]['lat'])
                coords.append([lon, lat])
                noms.append((nom, adresse, lon, lat))
            else:
                print(f"Adresse non trouvée : {adresse} {postal}")
                coords.append(None)

# === 4. Filtrage des coordonnées valides ===
coords_valides = [c for c in coords if c is not None]
noms_valides = [n for i, n in enumerate(noms) if coords[i] is not None]

N = len(coords_valides)
B = 10
distances = np.zeros((N, N), dtype=int)
durations = np.zeros((N, N), dtype=int)

# === 5. Appels à OpenRouteService pour générer la matrice de distances et durées ===
ors_url = "https://api.openrouteservice.org/v2/matrix/driving-car"
headers = {
    "Authorization": ORS_API_KEY,
    "Content-Type": "application/json"
}

for i_start in range(0, N, B):
    for j_start in range(0, N, B):
        i_end = min(i_start + B, N)
        j_end = min(j_start + B, N)

        sources = list(range(i_start, i_end))
        destinations = list(range(j_start, j_end))

        body = {
            "locations": coords_valides,
            "sources": sources,
            "destinations": destinations,
            "metrics": ["distance", "duration"],
            "units": "m"
        }

        response = requests.post(ors_url, json=body, headers=headers)
        time.sleep(1)

        if response.status_code != 200:
            print(f"Erreur ORS ({i_start}-{i_end}, {j_start}-{j_end}) :", response.status_code)
            print(response.text)
            exit(1)

        result = response.json()
        dist_block = np.array(result["distances"])
        dur_block = np.array(result["durations"])

        distances[i_start:i_end, j_start:j_end] = dist_block
        durations[i_start:i_end, j_start:j_end] = dur_block

        print(f"Bloc [{i_start}:{i_end}, {j_start}:{j_end}] rempli")

# === 6. Écriture des fichiers résultats ===
with open(os.path.join(output_dir, "pharmacies_etudiees.csv"), "w", newline="") as f:
    writer = csv.writer(f)
    for i, c in enumerate(coords):
        if c is not None:
            nom, adresse, lon, lat = noms[i]
            writer.writerow([nom, adresse, lon, lat])

with open(os.path.join(output_dir, "matrice_distances_m.csv"), "w", newline="") as f:
    for row in distances:
        row_str = ' '.join(str(int(val)) for val in row)
        f.write(row_str + '\n')

with open(os.path.join(output_dir, "matrice_durees_s.csv"), "w", newline="") as f:
    for row in durations:
        row_str = ' '.join(str(int(val)) for val in row)
        f.write(row_str + '\n')

print("Fichiers générés dans :", output_dir)

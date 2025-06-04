import requests
import numpy as np
import time
import csv
import math

ORS_API_KEY = "5b3ce3597851110001cf62481f7fe86c591e473c9e5474ca97c2263a"

coords = []
noms = []

# Étape 1 : Lecture + géocodage avec Nominatim
with open('livraison/livraison85.csv', newline='') as csvfile:
    for line in csvfile:
        line = line.strip()
        row = line.split(',')
        nom = row[0]
        adresse = row[1]
        postal = row[2]
        ville = row[3]

        # Appel à Nominatim
        adresse_complete = f"{adresse}, {postal} {ville}"
        url = "https://nominatim.openstreetmap.org/search"
        params = {
            "q": adresse_complete,
            "format": "json",
            "limit": 1
        }
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
            # Si l'adresse complète ne fonctionne pas, essayer avec ville et code postal
            adresseVille = f"{ville}, {postal}"
            url= "https://nominatim.openstreetmap.org/search"
            params = {
                "q": adresseVille,
                "format": "json",
                "limit": 1
            }
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
                print(f"Adresse non trouvée : {adresse} {postal}")
                coords.append(None)

# Étape 2 : Filtrage des coordonnées valides
coords_valides = [c for c in coords if c is not None]
noms_valides = [n for i, n in enumerate(noms) if coords[i] is not None]

N = len(coords_valides)
B = 10 
distances = np.zeros((N, N), dtype=int)
durations = np.zeros((N, N), dtype=int)


# Étape 3 : Appel à l’API OpenRouteService pour créer la matrice
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
        time.sleep(1)  # respecter les limites

        if response.status_code != 200:
            print(f"Erreur ORS ({i_start}-{i_end}, {j_start}-{j_end}) :", response.status_code)
            print(response.text)
            exit(1)

        result = response.json()

        dist_block = np.array(result["distances"])
        dur_block = np.array(result["durations"])

        distances[i_start:i_end, j_start:j_end] = dist_block
        durations[i_start:i_end, j_start:j_end] = dur_block

        print(f"✅ Bloc [{i_start}:{i_end}, {j_start}:{j_end}] rempli")


# Étape 4 : Écriture des résultats dans des CSV

# 1. Coordonnées valides avec nom et adresse
with open("livraison85/pharmacies_etudiees.csv", "w", newline="") as f:
    writer = csv.writer(f)
    for i, c in enumerate(coords):
        if c is not None:
            nom, adresse, lon, lat = noms[i]
            writer.writerow([nom, adresse, lon, lat])


# 2. Matrice des distances (en mètres)
with open("livraison85/matrice_distances_m.csv", "w", newline="") as f:
    for row in distances:
        row_str = ' '.join(str(int(val)) for val in row)
        f.write(row_str + '\n')

# 3. Matrice des durées (en secondes)
with open("livraison85/matrice_durees_s.csv", "w", newline="") as f:
    for row in durations:
        row_str = ' '.join(str(int(val)) for val in row)
        f.write(row_str + '\n')


print("- pharmacies_etudiees.csv")
print("- matrice_distances.csv")
print("- matrice_durees.csv")

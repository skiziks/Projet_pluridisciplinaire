import requests
import json

data_list = []
coords = []
count = 0

with open('livraison/livraison10.csv', newline='') as csvfile:
    for line in csvfile:
        line = line.strip()
        if not line:
            continue 
        row = line.split(',')
        nom = row[0]
        adresse = row[1]
        postal = row[2]
        ville = row[3]
        
        url = f"https://nominatim.openstreetmap.org/search?q={adresse}&format=json&limit=1"
        headers = {"User-Agent": "test/1.0 (email@mail.com)"}
        response = requests.get(url, headers=headers)
        data = response.json()

        if not data:
            url = f"https://nominatim.openstreetmap.org/search?q={ville}&format=json&limit=1"
            response = requests.get(url, headers=headers)
            data = response.json()
        
        lon = data[0]['lon']
        lat = data[0]['lat']

        nom_unique = f"{nom} {ville}"

        obj = {
            "id": count,
            "name": nom_unique,
            "adresse": adresse,
            "code": postal,
            "ville": ville,
            "coords": {
                "long": lon,
                "lat": lat
            },
            "path": []
        }
        data_list.append(obj)
        coords.append
        count += 1

        file = "data.json"
        with open(file, 'w') as fichier:
            json.dump(data_list, fichier, indent=2)
            

print(data_list)

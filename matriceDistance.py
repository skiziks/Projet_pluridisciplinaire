import requests
import numpy

coords = []

with open('livraison/livraison10.csv', newline='') as csvfile:
    for line in csvfile:
        line = line.strip()
        row = line.split(',')
        nom = row[0]
        adresse = row[1]
        postal = row[2]
        
        url = f"https://nominatim.openstreetmap.org/search?q={adresse} {postal}&format=json&limit=1"
        headers = {"User-Agent": "test/1.0 (email@mail.com)"}
        response = requests.get(url, headers=headers)
        
        data = response.json()
        if data:
            lon = data[0]['lon']
            lat = data[0]['lat']
        else:
            lon, lat = None, None
        
        coords.append([lon, lat])

print(coords)

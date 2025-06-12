import os
import sys
import pandas as pd
import matplotlib.pyplot as plt
import geopandas as gpd
from fpdf import FPDF
import contextily as ctx
import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)


# === 1. Récupération des arguments : dossier et fichier de routes ===
if len(sys.argv) != 3:
    print("Usage: python createPDF.py <dossier_livraison> <fichier_output.txt>")
    sys.exit(1)

folder = sys.argv[1]
output_file = sys.argv[2]
pharma_csv_path = os.path.join(folder, "pharmacies_etudiees.csv")

# === 2. Chargement des données des pharmacies ===
df_pharma = pd.read_csv(pharma_csv_path, header=None)
df_pharma.columns = ['Name', 'Address', 'Latitude', 'Longitude']

names = df_pharma['Name'].tolist()
addresses = df_pharma['Address'].tolist()
coords = list(zip(df_pharma['Latitude'].tolist(), df_pharma['Longitude'].tolist()))

# === 3. Lecture des routes optimisées ===
def read_routes(file_path):
    with open(file_path, 'r') as file:
        return [list(map(int, line.strip().split())) for line in file if line.strip()]

all_routes = read_routes(output_file)

# === 4. Génération d'un PDF avec carte et détails pour chaque route ===
for idx, route in enumerate(all_routes, start=1):
    # 4.1 Génération de la carte avec les points et les lignes de la route
    fig, ax = plt.subplots()

    gdf = gpd.GeoDataFrame(
        geometry=gpd.points_from_xy(
            [coords[i][0] for i in route],
            [coords[i][1] for i in route]
        )
    )
    gdf.plot(ax=ax, color='black', marker='o', markersize=40)

    for i in range(len(route) - 1):
        x1, y1 = coords[route[i]]
        x2, y2 = coords[route[i + 1]]
        plt.plot([x1, x2], [y1, y2], color='red', linewidth=2)

    ctx.add_basemap(ax, crs='EPSG:4326', source=ctx.providers.OpenStreetMap.Mapnik)
    ax.set_axis_off()

    map_filename = f'route_map_{idx}.png'
    plt.title(f'Optimized Pharmacy Delivery Route - Truck {idx}')
    plt.savefig(map_filename, dpi=300)
    plt.close()

    # 4.2 Création de la table des étapes de la route
    table_data = [
        [i + 1, names[route[i]], addresses[route[i]]]
        for i in range(len(route))
    ]
    df = pd.DataFrame(table_data, columns=['Order', 'Pharmacy', 'Address'])

    # 4.3 Génération du PDF avec carte et détails
    pdf = FPDF()
    pdf.add_page()
    pdf.set_font('Helvetica', 'B', 16)
    pdf.cell(200, 10, 'Pharmacy Delivery Route', align='C', ln=True)
    pdf.image(map_filename, x=10, y=20, w=190)

    pdf.set_y(150)
    pdf.set_font('Helvetica', 'B', 12)
    pdf.cell(200, 10, 'Route Details', ln=True)
    pdf.set_font('Helvetica', '', 12)

    for _, row in df.iterrows():
        pdf.cell(
            200,
            10,
            f"{row['Order']} - {row['Pharmacy']}, {row['Address']}",
            ln=True
        )

    pdf_filename = f'route_camion_{idx}.pdf'
    pdf.output(pdf_filename)
    print(f"PDF generated: {pdf_filename}")

import os
import pandas as pd
import matplotlib.pyplot as plt
import geopandas as gpd
from fpdf import FPDF, XPos, YPos

import contextily as ctx

# coords = []
# names = []
# addresses = []

# Read the pharmacy data from CSV
df_pharma = pd.read_csv("pharmacies_etudiees.csv")
df_pharma.columns = df_pharma.columns.str.strip()
df_pharma.columns = ['Name', 'Address', 'Latitude', 'Longitude']

names = df_pharma['Name'].tolist()
addresses = df_pharma['Address'].tolist()
coords = list(zip(df_pharma['Latitude'].tolist(), df_pharma['Longitude'].tolist()))

# with open('pharmacies_etudiees.csv', 'r') as file:
#     for line in file:
#         parts = line.strip().split(',')
#         names.append(parts[0])
#         addresses.append(parts[1])
#         coords.append((float(parts[2]), float(parts[3])))

# Read all routes
# with open('paths.txt', 'r') as file:
#     all_routes = [list(map(int, line.strip().split())) for line in file if line.strip()]
def read_routes(file_path):
    with open(file_path, 'r') as file:
        return [list(map(int, line.strip().split())) for line in file if line.strip()]

all_routes = read_routes('paths.txt')


# Generate a pdf for each route
for idx, route in enumerate(all_routes, start=1):
    # Generate map
    fig, ax = plt.subplots()

    gdf = gpd.GeoDataFrame(
        geometry=gpd.points_from_xy(
            [coords[i][0] for i in route], 
            [coords[i][1] for i in route])
    )
    gdf.plot(ax=ax, color='black', marker='o', markersize=40)


    # Draw the route
    for i in range(len(route) - 1):
        x1, y1 = coords[route[i]]
        x2, y2 = coords[route[i + 1]]
        plt.plot([x1, x2], [y1, y2], color='red', linewidth=2)

    ctx.add_basemap(ax, crs='EPSG:4326', source=ctx.providers.OpenStreetMap.Mapnik)
    ax.set_axis_off()

    # plt.title('Optimized Pharmacy Delivery Route')
    # plt.savefig('route_map.png', dpi=300)
    map_filename = f'route_map_{idx}.png'
    plt.title(f'Optimized Pharmacy Delivery Route - Truck {idx}')
    plt.savefig(map_filename, dpi=300)
    plt.close()

    # Create the summary table
    table_data = [
        [i + 1, names[route[i]], addresses[route[i]]] 
        for i in range(len(route))]
    df = pd.DataFrame(table_data, columns=['Order', 'Pharmacy', 'Address'])

    # Create PDF object
    pdf = FPDF()
    pdf.add_page()
    pdf.set_font('Helvetica', 'B', 16)  
    pdf.cell(200, 10, 'Pharmacy Delivery Route', new_x=XPos.LMARGIN, new_y=YPos.NEXT, align='C')
    pdf.image('route_map.png', x=10, y=20, w=190)

    pdf.set_y(150)
    pdf.set_font('Helvetica', 'B', 12) 
    pdf.cell(200, 10, 'Route Details', new_x=XPos.LMARGIN, new_y=YPos.NEXT)
    pdf.set_font('Helvetica', '', 12) 

    for _, row in df.iterrows():
        pdf.cell(
            200, 
            10, 
            f"{row['Order']} - {row['Pharmacy']}, {row['Address']}", 
            new_x=XPos.LMARGIN, 
            new_y=YPos.NEXT)


    # Save the PDF
    # pdf.output('route_summary_1.pdf')
    # print("PDF generated.")

    pdf_filename = f'route_camion_{idx}.pdf'
    pdf.output(pdf_filename)
    print(f"PDF generated: {pdf_filename}")



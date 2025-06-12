import os
import pandas as pd
import matplotlib.pyplot as plt
import geopandas as gpd
from fpdf import FPDF, XPos, YPos
import contextily as ctx
from PIL import Image


# Read the pharmacy data from CSV
df_pharma = pd.read_csv("livraison85/pharmacies_etudiees.csv", header=None)
df_pharma.columns = ['Name', 'Address', 'Latitude', 'Longitude']

names = df_pharma['Name'].tolist()
addresses = df_pharma['Address'].tolist()
coords = list(zip(df_pharma['Latitude'].tolist(), df_pharma['Longitude'].tolist()))

def read_routes(file_path):
    with open(file_path, 'r') as file:
        return [list(map(int, line.strip().split())) for line in file if line.strip()]

all_routes = read_routes('output.txt')

#####################################################################
####################             MAP             ####################
    


# Generate a pdf for each route
for idx, route in enumerate(all_routes, start=1):
    # Generate map
    fig, ax = plt.subplots(figsize=(6, 6), dpi=150)

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

    # Fit axis to route with padding 
    lats = [coords[i][0] for i in route]
    lons = [coords[i][1] for i in route]

    lat_min, lat_max = min(lats), max(lats)
    lon_min, lon_max = min(lons), max(lons)

    # Calculate center and max span
    lat_center = (lat_min + lat_max) / 2
    lon_center = (lon_min + lon_max) / 2
    lat_span = lat_max - lat_min
    lon_span = lon_max - lon_min
    max_span = max(lat_span, lon_span) * 1.2 or 0.01  # add margin

    # Set square bounds
    ax.set_xlim(lat_center - max_span / 2, lat_center + max_span / 2)
    ax.set_ylim(lon_center - max_span / 2, lon_center + max_span / 2)

    ctx.add_basemap(ax, crs='EPSG:4326', source=ctx.providers.OpenStreetMap.Mapnik)
    ax.set_axis_off()

    map_filename = f'route_map_{idx}.png'
    plt.title(f'Optimized Pharmacy Delivery Route - Truck {idx}')
    plt.tight_layout()
    plt.savefig(map_filename, dpi=300, bbox_inches='tight', pad_inches=0)
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

    # Insert image with fixed width, height calculated from aspect ratio
    pdf_width = 100
    with Image.open(map_filename) as img:
        img_width, img_height = img.size
        aspect = img_height / img_width
        pdf_height = pdf_width * aspect
        if pdf_height > 100:
            pdf_height = 100
            pdf_width = pdf_height / aspect

        pdf_x = (210 - pdf_width) / 2  # Center horizontally
        pdf.image(map_filename, x=pdf_x, y=30, w=pdf_width, h=pdf_height)

    #####################################################################
    ###################             TABLE             ###################
    
    
    # pdf.set_y(30 + pdf_height + 10)
    # pdf.set_font('Helvetica', 'B', 12) 
    # pdf.cell(200, 10, 'Route Details', new_x=XPos.LMARGIN, new_y=YPos.NEXT)
    # pdf.set_font('Helvetica', '', 12) 

    # for _, row in df.iterrows():
    #     pdf.cell(
    #         200, 
    #         10, 
    #         f"{row['Order']} - {row['Pharmacy']}, {row['Address']}", 
    #         new_x=XPos.LMARGIN, 
    #         new_y=YPos.NEXT)

    pdf.set_y(30 + pdf_height + 10)
    
    row_height = 7 
    col_widths = [70, 80, 20, 20] 

    # Header row
    pdf.set_font('Helvetica', 'B', 9)
    pdf.cell(col_widths[0], row_height, f"Parcours camionnette {idx}", border=1, align='C')
    pdf.cell(col_widths[1], row_height, "Address", border=1, align='C')
    pdf.cell(col_widths[2], row_height, "Arrival", border=1, align='C')
    pdf.cell(col_widths[3], row_height, "Departure", border=1, align='C')
    pdf.ln()

    # Content rows
    pdf.set_font('Helvetica', '', 9)
    for i in range(len(route)):
        name = names[route[i]]
        address = addresses[route[i]]

        # Custom labels for first and last rows
        if i == 0:
            name = "Départ entrepôt Cerp"
        elif i == len(route) - 1:
            name = "Retour entrepôt Cerp"

        pdf.cell(col_widths[0], row_height, name, border=1)
        pdf.cell(col_widths[1], row_height, address, border=1)
        pdf.cell(col_widths[2], row_height, "", border=1)
        pdf.cell(col_widths[3], row_height, "", border=1)
        pdf.ln(row_height)
        
    pdf_filename = f'route_camion_{idx}.pdf'
    pdf.output(pdf_filename)
    print(f"PDF generated: {pdf_filename}")



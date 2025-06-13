import os
import sys
import pandas as pd
import matplotlib.pyplot as plt
import geopandas as gpd
from fpdf import FPDF, XPos, YPos
import contextily as ctx
from PIL import Image
from datetime import datetime, timedelta, time
import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)


#####################################################################
#################             LOAD DATA             #################


if len(sys.argv) != 3:
    print("Usage: python createPDF.py <dossier_livraison> <fichier_output.txt>")
    sys.exit(1)

folder = sys.argv[1]
output_file = sys.argv[2]
pharma_csv_path = os.path.join(folder, "pharmacies_etudiees.csv")
df_times_path = os.path.join(folder, "matrice_durees_s.csv")
df_dist_path = os.path.join(folder, "matrice_distances_m.csv")

# Load pharmacies
df_pharma = pd.read_csv(pharma_csv_path, header=None)
df_pharma.columns = ['Name', 'Address', 'Latitude', 'Longitude']

names = df_pharma['Name'].tolist()
addresses = df_pharma['Address'].tolist()
coords = list(zip(df_pharma['Latitude'].tolist(), df_pharma['Longitude'].tolist()))

# Load travel time 
df_times = pd.read_csv(df_times_path, sep=' ', header=None)

#Load distances
df_dist = pd.read_csv(df_dist_path, sep=' ', header=None)

# Load routes 
def read_routes(file_path):
    with open(file_path, 'r') as file:
        return [list(map(int, line.strip().split())) for line in file if line.strip()]

all_routes = read_routes(output_file)


#####################################################################
##############             DETERMINE  TIME             ##############

now = datetime.now()
today = now.date()
if now < datetime.combine(today, time(9, 0)):
    shift_start_dt = datetime.combine(today, time(9, 0))
elif now < datetime.combine(today, time(13, 0)):
    shift_start_dt = datetime.combine(today, time(15, 0))
else:
    shift_start_dt = datetime.combine(today, time(9, 0))


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

        # Coordonnées du centre
        xm = (x1 + x2) / 2
        ym = (y1 + y2) / 2

        # Petite flèche au milieu
        dx = x2 - x1
        dy = y2 - y1
        ax.annotate(
            '',  
            xy=(xm + 0.0001 * dx, ym + 0.0001 * dy),  
            xytext=(xm - 0.0001 * dx, ym - 0.0001 * dy),  
            arrowprops=dict(arrowstyle="->", color='red', lw=1.5)
        )

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
    max_span = max(lat_span, lon_span) * 1.2 or 0.01

    # Set square bounds
    ax.set_xlim(lat_center - max_span / 2, lat_center + max_span / 2)
    ax.set_ylim(lon_center - max_span / 2, lon_center + max_span / 2)

    ctx.add_basemap(ax, crs='EPSG:4326', source=ctx.providers.OpenStreetMap.Mapnik)

    depot_x, depot_y = coords[route[0]]
    ax.plot(depot_x, depot_y, marker='o', markersize=15, color='blue', zorder=3)

    map_filename = f'route_map_{idx}.png'
    plt.title(f'Delivery Route - Truck {idx}')
    plt.tight_layout()
    plt.savefig(map_filename, dpi=300, bbox_inches='tight', pad_inches=0)
    plt.close()

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

        pdf_x = (210 - pdf_width) / 2
        pdf.image(map_filename, x=pdf_x, y=30, w=pdf_width, h=pdf_height)


    #####################################################################
    ###########             SCHEDULE  CALCULATION             ###########

    def compute_schedule(routes, matrix, delivery_duration=180, shift_start=None):
        all_schedules = []
        for route in routes:
            current_time = shift_start
            schedule = []
            for i in range(len(route)):
                if i == 0:
                    arrival = current_time
                    departure = current_time
                else:
                    travel_time = int(matrix.iloc[route[i-1], route[i]])
                    arrival = current_time + timedelta(seconds=travel_time)
                    departure   = arrival + timedelta(seconds=delivery_duration)
                schedule.append((arrival.strftime("%H:%M"), departure.strftime("%H:%M")))
                current_time = departure
            all_schedules.append(schedule)
        return all_schedules

    all_schedules = compute_schedule(all_routes, df_times, shift_start=shift_start_dt)


    #####################################################################
    ###################             TABLE             ###################

    pdf.set_y(30 + pdf_height + 10)
    row_height = 7 
    col_widths = [70, 80, 20, 20] 

    pdf.set_font('Helvetica', 'B', 9)
    pdf.cell(col_widths[0], row_height, f"Parcours camionnette {idx}", border=1, align='C')
    pdf.cell(col_widths[1], row_height, "Address", border=1, align='C')
    pdf.cell(col_widths[2], row_height, "Arrival", border=1, align='C')
    pdf.cell(col_widths[3], row_height, "Departure", border=1, align='C')
    pdf.ln()

        # Content rows
    pdf.set_font('Helvetica', '', 9)
    for i in range(len(route)):
        pharmacy_index = route[i]
        name = names[pharmacy_index].replace("Œ", "OE")
        address = addresses[pharmacy_index].replace("’", "'")

        arrival, departure = all_schedules[idx - 1][i]

        if i == 0:
            name = "Départ entrepôt Cerp"
            arrival = ""
        elif i == len(route) - 1:
            name = "Retour entrepôt Cerp"
            departure = ""

        pdf.cell(col_widths[0], row_height, name, border=1)
        pdf.cell(col_widths[1], row_height, address, border=1)
        pdf.cell(col_widths[2], row_height, arrival, border=1, align='C')
        pdf.cell(col_widths[3], row_height, departure, border=1, align='C')
        pdf.ln(row_height)



    #####################################################################
    ###########              DISTANCE + FUEL INFO             ###########

    total_distance_m = sum(df_dist.iloc[route[i], route[i + 1]] for i in range(len(route) - 1))
    total_distance_km = total_distance_m / 1000
    fuel_consumed = total_distance_km * (8.5 / 100)
    fuel_cost = fuel_consumed * 1.72

    pdf.ln(5)
    pdf.set_font('Helvetica', 'B', 11)
    pdf.cell(0, 8, f"Total distance : {total_distance_km:.0f} km", new_x=XPos.LMARGIN, new_y=YPos.NEXT)
    pdf.cell(0, 8, f"Estimated fuel consumption : {fuel_consumed:.2f}L (8.5 L / 100km)", new_x=XPos.LMARGIN, new_y=YPos.NEXT)
    pdf.cell(0, 8, f"Fuel price : {fuel_cost:.2f} euros (diesel : 1.72 euros / L)", new_x=XPos.LMARGIN, new_y=YPos.NEXT)

    pdf_filename = f'route_camion_{idx}.pdf'
    pdf.output(pdf_filename)
    print(f"PDF generated: {pdf_filename}")

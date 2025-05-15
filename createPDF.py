import os
import pandas as pd
import matplotlib.pyplot as plt
import geopandas as gpd
from fpdf import FPDF, XPos, YPos

import contextily as ctx

# Example
coords = [
    (2.3522, 48.8566),      # Pharmacy 0
    (2.2945, 48.8584),      # Pharmacy 1 
    (2.3499, 48.864716),    # Pharmacy 2 
    (2.3608, 48.8857),      # Pharmacy 3 
    (2.3333, 48.8606)       # Pharmacy 4 
]

# Read the route
with open('route_result.txt', 'r') as file:
    route = list(map(int, file.readline().strip().split(',')))

# Generate the map with the route
fig, ax = plt.subplots()

# Add the map
gdf = gpd.GeoDataFrame(
    geometry=gpd.points_from_xy([coords[i][0] for i in route], [coords[i][1] for i in route])
)
gdf.plot(ax=ax, color='green', marker='o', markersize=50)


# Draw the route
for i in range(len(route) - 1):
    x1, y1 = coords[route[i]]
    x2, y2 = coords[route[i + 1]]
    plt.plot([x1, x2], [y1, y2], color='red', linewidth=2)

ctx.add_basemap(ax, crs='EPSG:4326', source=ctx.providers.OpenStreetMap.Mapnik)
ax.set_axis_off()

plt.title('Optimized Pharmacy Delivery Route')
plt.savefig('route_map.png', dpi=300)

# Create the summary table
table_data = [[f'Point {i}', coords[route[i]][0], coords[route[i]][1]] for i in range(len(route))]
df = pd.DataFrame(table_data, columns=['Order', 'Longitude', 'Latitude'])

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

# Add the table 
for _, row in df.iterrows():
    pdf.cell(200, 10, f"{row['Order']} - {row['Longitude']}, {row['Latitude']}", new_x=XPos.LMARGIN, new_y=YPos.NEXT)

# Save the PDF
pdf.output('route_summary.pdf')

print("PDF generated.")


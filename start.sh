#!/bin/bash

if ! command -v python3 >/dev/null 2>&1; then
  sudo apt update
  sudo apt install -y python3 python3-pip python3-venv
fi

sudo apt install -y \
  make g++ \
  libgeos-dev libspatialindex-dev libproj-dev libgdal-dev

if [ -d "venv" ]; then
  rm -rf venv
fi

python3 -m venv venv
if [ $? -ne 0 ]; then
  exit 1
fi

source venv/bin/activate

venv/bin/pip install --upgrade pip --break-system-packages > /dev/null
venv/bin/pip install --break-system-packages requests numpy geopandas matplotlib pandas contextily fpdf2 > /dev/null

if [ $? -ne 0 ]; then
  deactivate
  exit 2
fi

echo "Environnement prêt. Lancer ./pipeline.sh <fichier_csv>"

import pandas as pd
import os
import sys
import re
import math
import unicodedata
import csv

def parse_coordinates_string(coord_str):
    """
    Parse une chaîne de caractères de coordonnées.
    """
    match = re.match(r'\[(-?\d+\.?\d*),(-?\d+\.?\d*)\]', coord_str)
    if match:
        return float(match.group(1)), float(match.group(2))
    raise ValueError(f"Format de chaîne de coordonnées invalide: {coord_str}. Attendu '[longitude,latitude]'.")

def normalize_string(s):
    """
    Normalise une chaîne de caractères pour la recherche (minuscules, sans accents).
    """
    if s is None:
        return ""
    s = str(s).strip().lower()
    s = re.sub(r'[\s\-_/]+', ' ', s)
    s = s.strip()
    s = unicodedata.normalize('NFKD', s).encode('ascii', 'ignore').decode('utf-8')
    return s

def find_column_name(df_columns, possible_names):
    """
    Trouve le nom de colonne réel dans un DataFrame à partir d'une liste de noms possibles.
    """
    normalized_df_columns = {normalize_string(col): col for col in df_columns}
    for name in possible_names:
        normalized_name = normalize_string(name)
        if normalized_name in normalized_df_columns:
            return normalized_df_columns[normalized_name]
    return None

def generate_matrices(input_csv_path):
    """
    Fonction principale pour générer les matrices de distances et de durées
    ainsi que le fichier des pharmacies étudiées à partir du CSV d'entrée.
    """
    print(f"--- Début du script matriceDistance.py pour : {input_csv_path} ---")

    # Configuration des chemins de fichiers et création du dossier de sortie.
    script_dir = os.path.dirname(os.path.abspath(__file__))
    geocache_path = os.path.join(script_dir, 'geocache.csv')
    distance_matrix_full_path = os.path.join(script_dir, 'distance_matrix.csv')
    time_matrix_full_path = os.path.join(script_dir, 'time_matrix.csv')

    csv_filename = os.path.basename(input_csv_path)
    folder_name = os.path.splitext(csv_filename)[0]
    output_folder_path = os.path.join(script_dir, folder_name)

    try:
        os.makedirs(output_folder_path, exist_ok=True)
    except OSError as e:
        print(f"Erreur lors de la création du dossier de sortie '{output_folder_path}': {e}", file=sys.stderr)
        sys.exit(1)

    # Chargement et préparation des données de géocaching, qui servent de référence.
    try:
        geocache_df = pd.read_csv(geocache_path, encoding='utf-8')
        geocache_df['original_matrix_idx'] = geocache_df.index + 1 
        geocache_df[['Longitude', 'Latitude']] = geocache_df['[Longitude,Latitude]'].apply(
            lambda x: pd.Series(parse_coordinates_string(x))
        )
        geocache_df.drop(columns=['[Longitude,Latitude]'], inplace=True)

    except FileNotFoundError:
        print(f"Erreur : Le fichier geocache.csv est introuvable à : {geocache_path}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Erreur lors du chargement ou du traitement de geocache.csv : {e}", file=sys.stderr)
        sys.exit(1)

    # Identification des colonnes essentielles pour la recherche.
    geocache_name_col = find_column_name(geocache_df.columns, ['Name', 'nom', 'NOM', 'pharmacie', 'Pharmacie'])
    geocache_address_col = find_column_name(geocache_df.columns, ['Address', 'adresse', 'ADRESSE', 'addr'])
    geocache_postal_code_col = find_column_name(geocache_df.columns, ['Postal_Code', 'PostalCode', 'code postal', 'Code Postal', 'CODE POSTAL', 'code_postal', 'Code_Postal', 'postcode', 'Postcode', 'CP'])
    geocache_city_col = find_column_name(geocache_df.columns, ['City', 'ville', 'VILLE'])

    if not all([geocache_name_col, geocache_address_col, geocache_postal_code_col, geocache_city_col]):
        print("Erreur : Impossible de trouver toutes les colonnes essentielles (Nom, Adresse, Code Postal, Ville) dans geocache.csv.", file=sys.stderr)
        sys.exit(1)

    # Création d'un dictionnaire de recherche rapide pour les données de géocaching.
    geocache_lookup = {}
    for _, row in geocache_df.iterrows():
        key = (
            normalize_string(row[geocache_name_col]),
            normalize_string(row[geocache_address_col]),
            normalize_string(row[geocache_postal_code_col]),
            normalize_string(row[geocache_city_col])
        )
        geocache_lookup[key] = {
            'Name': row[geocache_name_col],
            'Address': row[geocache_address_col],
            'Postal_Code': row[geocache_postal_code_col],
            'City': row[geocache_city_col],
            'Longitude': row['Longitude'],
            'Latitude': row['Latitude'],
            'original_matrix_idx': row['original_matrix_idx']
        }

    # Chargement des pharmacies à livrer à partir du fichier d'entrée et association des données.
    pharmacies_to_deliver = []
    pharmacies_etudiees_data = []

    try:
        input_pharmacies_df = pd.read_csv(input_csv_path, encoding='utf-8', header=None)
        
        if input_pharmacies_df.empty:
            sys.exit(0)

        if input_pharmacies_df.shape[1] < 4:
            print(f"Erreur : Le fichier d'entrée {input_csv_path} doit contenir au moins 4 colonnes (nom, adresse, code postal, ville).", file=sys.stderr)
            sys.exit(1)

        # Gestion spécifique de l'entrepôt "CERP" pour s'assurer qu'il est le premier point (index 0).
        canonical_depot_key = (
            normalize_string("CERP"),
            normalize_string("600 Rue des Madeleines"),
            normalize_string("77100"),
            normalize_string("Mareuil-les-Meaux")
        )
        depot_cerp_info = geocache_lookup.get(canonical_depot_key)
        
        if depot_cerp_info:
            pharmacies_to_deliver.append(depot_cerp_info)
            pharmacies_etudiees_data.append({
                'Nom': depot_cerp_info['Name'],
                'Adresse': depot_cerp_info['Address'],
                'Longitude': depot_cerp_info['Longitude'],
                'Latitude': depot_cerp_info['Latitude']
            })
        else:
            print("Erreur : L'entrepôt 'CERP' n'a pas été trouvé dans geocache.csv. Il est essentiel pour le fonctionnement.", file=sys.stderr)
            sys.exit(1)

        # Itération sur les pharmacies du fichier d'entrée pour les associer.
        for index, row in input_pharmacies_df.iterrows():
            input_key = (
                normalize_string(row[0]),
                normalize_string(row[1]),
                normalize_string(row[2]),
                normalize_string(row[3])
            )
            found_pharmacy = geocache_lookup.get(input_key)

            if found_pharmacy:
                if depot_cerp_info and found_pharmacy['original_matrix_idx'] == depot_cerp_info['original_matrix_idx']:
                    continue
                
                pharmacies_to_deliver.append(found_pharmacy)
                pharmacies_etudiees_data.append({
                    'Nom': found_pharmacy['Name'],
                    'Adresse': found_pharmacy['Address'],
                    'Longitude': found_pharmacy['Longitude'],
                    'Latitude': found_pharmacy['Latitude']
                })
        
        if len(pharmacies_to_deliver) <= 1:
            pass # No specific warning printed for 1 or fewer pharmacies
            
        print(f"Pharmacies à livrer : {len(pharmacies_to_deliver)} pharmacies.")

    except FileNotFoundError:
        print(f"Erreur : Le fichier d'entrée est introuvable à : {input_csv_path}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Erreur inattendue lors du chargement des pharmacies d'entrée : {e}", file=sys.stderr)
        sys.exit(1)

    # Chargement des matrices complètes de distances et de durées.
    # Ces matrices contiennent toutes les relations possibles entre les points du geocache.
    try:
        dist_matrix_full_df = pd.read_csv(distance_matrix_full_path, index_col=0, header=0, encoding='utf-8', sep=',')
        dist_matrix_full_df.index = dist_matrix_full_df.index.astype(int)
        dist_matrix_full_df.columns = dist_matrix_full_df.columns.astype(int)

        time_matrix_full_df = pd.read_csv(time_matrix_full_path, encoding='utf-8', sep=',', header=None, skiprows=[0])
        time_matrix_row_ids = time_matrix_full_df.iloc[:, 0].astype(int)
        time_matrix_data = time_matrix_full_df.iloc[:, 1:]
        time_matrix_full_df = pd.DataFrame(time_matrix_data.values, 
                                           index=time_matrix_row_ids, 
                                           columns=range(1, time_matrix_data.shape[1] + 1))
        time_matrix_full_df.columns = time_matrix_full_df.columns.astype(int)
        time_matrix_full_df = time_matrix_full_df.astype(float)
        
        all_expected_ids = range(1, dist_matrix_full_df.shape[0] + 1)
        time_matrix_full_df = time_matrix_full_df.reindex(index=all_expected_ids, columns=all_expected_ids, fill_value=0.0)

        # Imputation des temps pour l'entrepôt si les valeurs sont manquantes ou nulles.
        depot_original_idx = depot_cerp_info['original_matrix_idx']
        if time_matrix_full_df.loc[depot_original_idx].sum() < 0.01:
            depot_distances_km = dist_matrix_full_df.loc[depot_original_idx, :].copy()
            average_speed_kps = 30 / 3600 # 30 km/h en km/s

            for col_id in all_expected_ids:
                if col_id == depot_original_idx:
                    time_matrix_full_df.loc[depot_original_idx, col_id] = 0.0
                else:
                    estimated_time_min = depot_distances_km.loc[col_id] / average_speed_kps / 60
                    time_matrix_full_df.loc[col_id, depot_original_idx] = estimated_time_min
                    if time_matrix_full_df.loc[col_id, depot_original_idx] == 0.0:
                        time_matrix_full_df.loc[col_id, depot_original_idx] = estimated_time_min
            
    except FileNotFoundError as e:
        print(f"Erreur : Un fichier matrice est introuvable ({e.filename}). Assurez-vous que 'distance_matrix.csv' et 'time_matrix.csv' sont dans le même répertoire que le script.", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Erreur inattendue lors du chargement des matrices complètes : {e}", file=sys.stderr)
        sys.exit(1)

    # Construction des nouvelles matrices de distances et de durées.
    # Ces matrices sont spécifiques aux pharmacies à livrer et sont au format attendu par l'algorithme C.
    num_pharmacies = len(pharmacies_to_deliver)

    new_dist_matrix = pd.DataFrame(0, index=range(num_pharmacies), columns=range(num_pharmacies), dtype=int)
    new_time_matrix = pd.DataFrame(0, index=range(num_pharmacies), columns=range(num_pharmacies), dtype=int)

    for i in range(num_pharmacies):
        idx_i_original = pharmacies_to_deliver[i]['original_matrix_idx'] 
        for j in range(num_pharmacies):
            idx_j_original = pharmacies_to_deliver[j]['original_matrix_idx']
            
            try:
                dist_val_float_km = dist_matrix_full_df.loc[idx_i_original, idx_j_original]
                time_val_float_min = time_matrix_full_df.loc[idx_i_original, idx_j_original]

                converted_dist_m = dist_val_float_km * 1000
                converted_time_s = time_val_float_min * 60

                new_dist_matrix.iloc[i, j] = int(round(converted_dist_m))
                new_time_matrix.iloc[i, j] = int(round(converted_time_s))

                if i == j: # Assure que la diagonale est à zéro
                    new_dist_matrix.iloc[i, j] = 0
                    new_time_matrix.iloc[i, j] = 0

            except KeyError as e:
                print(f"Erreur: Index de matrice '{e}' non trouvé dans les matrices complètes. Veuillez vérifier les données.", file=sys.stderr)
                sys.exit(1)
            except Exception as e:
                print(f"Erreur lors de la récupération des valeurs matricielles : {e}", file=sys.stderr)
                sys.exit(1)
    print("Matrices de distances et de durées spécifiques générées avec succès.")

    # Sauvegarde des fichiers de sortie : les matrices et le fichier des pharmacies étudiées.
    output_dist_matrix_path = os.path.join(output_folder_path, 'matrice_distances_m.csv')
    output_time_matrix_path = os.path.join(output_folder_path, 'matrice_durees_s.csv')
    output_pharmacies_etudiees_path = os.path.join(output_folder_path, 'pharmacies_etudiees.csv')

    try:
        new_dist_matrix.to_csv(output_dist_matrix_path, sep=' ', float_format='%.0f', header=False, index=False, encoding='utf-8')
        new_time_matrix.to_csv(output_time_matrix_path, sep=' ', float_format='%.0f', header=False, index=False, encoding='utf-8')

        with open(output_pharmacies_etudiees_path, 'w', newline='', encoding='utf-8') as f:
            writer = csv.writer(f, quoting=csv.QUOTE_ALL)
            for pharma_data in pharmacies_etudiees_data:
                writer.writerow([
                    pharma_data['Nom'],
                    pharma_data['Adresse'],
                    pharma_data['Longitude'],
                    pharma_data['Latitude']
                ])

    except Exception as e:
        print(f"Erreur lors de la sauvegarde des fichiers de sortie : {e}", file=sys.stderr)
        sys.exit(1)

    print(f"--- Script matriceDistance.py terminé avec succès pour {csv_filename} ---")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python matriceDistance.py <chemin_du_fichier_CSV_d_entree>", file=sys.stderr)
        sys.exit(1)
    
    input_csv_file = sys.argv[1]
    generate_matrices(input_csv_file)

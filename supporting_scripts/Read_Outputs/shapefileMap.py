import geopandas as gpd
import numpy as np
import matplotlib.pyplot as plt
import contextily as cx

# open it...
# Shapefile
shpfile_folder = '/Users/diogocosta/Library/CloudStorage/OneDrive-impactblue-scientific.com/6_Projects/1_GWF/2_WIP/code/mizuRoute/case_studies/testCase_cameo_v1.2/shapefile/shapefiles/'
shpfile_file = 'Flowline_CO_14_cameo.shp'
shpfile_fullpath = shpfile_folder + shpfile_file

geodf = gpd.read_file(shpfile_fullpath)
geodf = geodf.to_crs("WGS84")

# Looping over time
for i in range(10):

    # Addin WQ
    geodf["openwq_wq"] = geodf["LengthKM"] * 0 + i

    # Plotting
    ax = geodf.plot(column='openwq_wq', legend=True, vmin=0, vmax=10, figsize=(20, 14))
    cx.add_basemap(ax, source=cx.providers.Stamen.TonerLite)
    plt.draw()
    # plt.pause(0.05)
    plt.savefig('gif_results/' + str(i) + '.png')

# Build GIF
import imageio.v2

with imageio.get_writer('gif_results/results_openwq.gif', mode='I') as writer:
    for t in range(10):
        image = imageio.v2.imread('gif_results/' + str(t) + '.png')
        writer.append_data(image)

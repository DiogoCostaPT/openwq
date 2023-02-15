import netCDF4
import h5py
import geopandas as gpd
import numpy as np
import matplotlib.pyplot as plt
import contextily as cx
from datetime import datetime
import os
import shutil

def MapGeoPandas(shpfile_fullpath,
                openwq_out_fullpath,
                mizuroute_out_fullpath,
                folderSaveFigs_fullpath,
                gif_name_fullpath):

    # Shapefile River
    geodf = gpd.read_file(shpfile_fullpath)
    geodf = geodf.to_crs("WGS84")
    comid = np.array(geodf['ComID'])

    # Adding WQ field in shapegfile
    geodf  ["openwq_wq"] = geodf["LengthKM"] * 0

    # Get NETOP info
    #netop_subdir = 'ancillary_data/'
    #netop_file = 'ntopo_nhdplus_cameo_pfaf.nc'
    #netcdf_file = netCDF4.Dataset(mainModelDir + netop_subdir + netop_file)
    #netopo = netcdf_file.variables['to'] # netopo key
    #netopo = np.array(netopo)

    # Openwq h5 results
    hf_file = h5py.File(openwq_out_fullpath, 'r')
    # get list of h5 keys (timestamps)
    keys_list = list(hf_file.keys())
    keys_list = keys_list[1:len(keys_list)-2]  # need to remove the last two datasets (xyz_elements)
    # convert timestring to datetime
    datetime_timestampts = [datetime.strptime(keys_list[i], '%Y%b%d-%H:%M:%S') for i in range(len(keys_list))]
    # sort datetime and get indexes
    indices = sorted(
        range(len(datetime_timestampts)),
        key=lambda index: datetime_timestampts[index]
    )

    # Get mapping of indexes
    #indexes = np.array()
    #for i in range(len(comid)):
    #    comid_i = comid[i]
    #    index_i = np.where()

    # mizuroute results
    mizuroute_out_file = netCDF4.Dataset(mizuroute_out_fullpath)
    mizuroute_out_idFeature = mizuroute_out_file.variables['reachID']
    mizuroute_out_idFeature = np.array(mizuroute_out_idFeature)

    # Make fig_gif folder
    if not os.path.exists(folderSaveFigs_fullpath):
        os.mkdir(folderSaveFigs_fullpath)

    # Looping over time
    for i in range(len(keys_list)):

        # Get data from timestamp i
        h5_timestamp_vals = hf_file.get(keys_list[indices[i]])
        h5_timestamp_vals = np.array(h5_timestamp_vals)

        # loop over shapefile elements and get comid
        # then find the correst output result
        for j in range(len(comid)):
            comid_j = comid[j]
            index_j = np.where(mizuroute_out_idFeature == comid_j)
            try:
                geodf["openwq_wq"][j] = h5_timestamp_vals[0][index_j[0]]
            except:
                geodf["openwq_wq"][j] = -1

        # Plotting
        ax = geodf.plot(column='openwq_wq', legend=True, vmin=0, vmax=0.15,
            legend_kwds={
                "location":"right",
                "shrink":.55
            },
        cmap="RdYlGn_r",figsize=(25, 20))

        plt.title(keys_list[indices[i]])

        #cx.add_basemap(ax, source=cx.providers.Stamen.TonerLite)
        # plt.pause(0.05)
        plt.savefig(folderSaveFigs_fullpath + str(i) + '.png')
        plt.close()

    # Build GIF
    import imageio.v2
    with imageio.get_writer(gif_name_fullpath, mode='I') as writer:
        for t in range(len(keys_list) + 15):
            image = imageio.v2.imread(folderSaveFigs_fullpath + str(min(t, len(keys_list))) + '.png')
            writer.append_data(image)

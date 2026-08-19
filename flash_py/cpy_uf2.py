# import the module
import shutil

# Specify the path of the file you want to copy to pico/dir 
file_to_copy = '../build/comm_protocols/lcd_i2c/lcd_i2c.uf2'

#Path of D: where .uf2 file is copied
destination_directory = 'D:'

# Use the shutil.copy() method to copy the file to the destination directory
shutil.copy(file_to_copy, destination_directory)
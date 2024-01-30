import pandas as pd

# Load the data
data = pd.read_csv('/home/ubuntu/git-repos/autonomus_driver/frames/training_data/training_data.csv', header=None, sep=" ")

# Perform min-max normalization
min_values = data.min()
max_values = data.max()
print(min_values)
print(max_values)
normalized_data = (data - min_values) / (max_values - min_values)

# Save the normalized data
normalized_data.to_csv('normalized_data.csv', header=False, index=False)

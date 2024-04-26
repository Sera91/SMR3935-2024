import numpy as np
import pandas as pd
import scipy as stats
from scipy.stats import chi2
  

#1st exercise:
#Once downloaded the LEAF dataset  from the webpage: http://archive.ics.uci.edu/ml/datasets/Leaf
#rank the importance of the variables for classifying the species according with their linear
#correlation coefficient.

def linear_correlation(col1, col2):
    """
    function to compute linear correlation
    coefficient associated to 2 numeric features
    """
    d1 = col1 - np.mean(col1)
    d2 = col2 - np.mean(col2)
    v1 = np.sum(np.square(d1))
    v2 = np.sum(np.square(d2))
    cov = np.sum(np.multiply(d1,d2))
    return cov/np.sqrt(v1*v2)

#Linear correlation coeff estimation
def linear_correlation_mod(xx,yy):
    x_diff= xx - np.mean(xx)
    y_diff = yy - np.mean(yy)


# calculateMahalanobis function to calculate
# the Mahalanobis distance
def Mahalanobis_dist(y=None, data=None, cov=None):
    y_mu = y - np.mean(data)
    if not cov:
        cov = np.cov(data.values.T)
    inv_covmat = np.linalg.inv(cov)
    left = np.dot(y_mu, inv_covmat)
    mahal = np.dot(left, y_mu.T)
    return mahal.diagonal()



################################################MAIN##############################################
#Importing the data
cols = ["Species", "Specimen_number", "Eccentricity", "Aspect_ratio", "Elongation", "solidity", 
           "stochastic_convexity", "isoperimetric_factor", "max_indent_depth", "lobedness", 
           "avg_intensity", "avg_contrast", "smoothness", "third_moment", "uniformity", "entropy"]
leaves_df = pd.read_csv("leaf/leaf.csv", names=cols)
leaves_df.head()


# check agreement with numpy value (up to machine precision)

print("Testing correlation coeff routine:")

print(f"Manual implementation: {linear_correlation(leaves_df['Species'], leaves_df['Eccentricity'])}")
print(f"Numpy implementation:  {np.corrcoef(leaves_df['Species'], leaves_df['Eccentricity'])[0][1]}")

# compute linear correlation to rank dataset features respect to the species
_ranks = []
_cols  = leaves_df.columns[1:]
for feature in _cols:
    _ranks.append(linear_correlation(leaves_df['Species'], leaves_df[feature]))
    
idx = np.argsort(_ranks)[::-1]
for i in idx:
    print(f"{_cols[i]: <30}: {_ranks[i]}")

#Selecting the first 5 feature for correlation rank
leaves_df_selected= leaves_df.loc[:,_cols[idx][0:5]]

print("We are going to estimate the mahalanobis distance between points in the dataset considering only these features:")
print(leaves_df_selected.columns)

#estimating the mahalanobis distance between points in the dataset considering only these features
leaves_df_selected['Mahalanobis'] = Mahalanobis_dist(leaves_df_selected, data=leaves_df_selected)

# calculate p-value for each mahalanobis distance
leaves_df_selected['p'] = 1 - chi2.cdf(leaves_df_selected['Mahalanobis'], 3)

print('Mahalanobis distance arr:')
print(leaves_df_selected['Mahalanobis'])

output_file="outputs/df_leaves_plus_Mahalanobis.txt"
with open(output_file, 'a') as f:
    dfAsString = leaves_df_selected.to_string(header=True, index=False)
    f.write(dfAsString)










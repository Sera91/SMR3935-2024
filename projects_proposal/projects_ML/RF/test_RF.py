from sklearn import datasets
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from serial_RF import RandomForest
import numpy as np

iris = datasets.load_iris(as_frame=True)

X = iris.data
Y = iris.target

X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.2)

N_obs=len(X_train)
print(N_obs)

n_features=len(X_train.columns.to_list())
print(n_features)
clf_scikit = RandomForestClassifier()
clf_scikit.fit(X_train, Y_train)
preds_scikit = clf_scikit.predict(X_test)
acc = sum(preds_scikit == Y_test) / len(Y_test)
print("Testing accuracy of scikit version: {}".format(np.round(acc,3)))

# Create a new Random Forests classifier using the imputed data
clf = RandomForest(n_trees=100, max_depth=10)
clf.fit(X_train, Y_train)
# Make predictions and evaluate

#print(len(X_test))

y_pred = clf.predict(X_test.to_numpy())
acc = sum(y_pred == Y_test) / len(Y_test)
print("Testing accuracy of our version: {}".format(np.round(acc,3)))


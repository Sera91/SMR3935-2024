import math
import numpy as np
from scipy import stats


class Node:
    """
    Represents a node in a decision tree.

    Attributes:
        gini (float): The Gini impurity of the node.
        num_samples (int): The total number of samples in the node.
        num_samples_per_class (list): The number of samples per class in the node.
        predicted_class (int): The predicted class for the node.
        feature_index (int): The index of the feature used for splitting the node.
        threshold (float): The threshold value used for splitting the node.
        left (Node): The left child node.
        right (Node): The right child node.
    """

    def __init__(self, gini, num_samples, num_samples_per_class, predicted_class):
        self.gini = gini
        self.num_samples = num_samples
        self.num_samples_per_class = num_samples_per_class
        self.predicted_class = predicted_class
        self.feature_index = 0
        self.threshold = 0
        self.left = None
        self.right = None



class DecisionTreeClassifier:
    """
    Decision tree classifier.

    Parameters
    ----------
    max_depth : int, optional (default=None)
        The maximum depth of the tree. If None, then nodes are expanded until all leaves are pure.
    min_samples_leaf : int, optional (default=1)
        The minimum number of samples required to be at a leaf node.

    Attributes
    ----------
    n_classes_ : int
        The number of classes.
    n_features_ : int
        The number of features when `fit` is performed.
    tree_ : Node
        The root of the decision tree.

    Methods
    -------
    fit(X, y)
        Build a decision tree classifier from the training set (X, y).
    predict(X)
        Predict class for X.
    _gini(y)
        Compute Gini impurity of a non-empty node.
    _best_split(X, y)
        Find the best split for a node.
    _grow_tree(X, y, depth)
        Build a decision tree by recursively finding the best split.
    _predict(inputs)
        Predict class for a single sample.
    """
    def __init__(self, max_depth=None, min_samples_leaf=1):
        self.max_depth = max_depth
        self.min_samples_leaf = min_samples_leaf
        self.tree_ = None

    def fit(self, X, y):
        self.n_classes_ = len(set(y))
        self.n_features_ = X.shape[1]
        self.tree_ = self._grow_tree(X, y)

    def predict(self, X):
        return [self._predict(inputs) for inputs in X]

    def _gini(self, y):
        m = len(y)
        return 1.0 - sum((np.sum(y == c) / m) ** 2 for c in range(self.n_classes_))

    def _best_split(self, X, y):
        m, n = X.shape
        if m <= self.min_samples_leaf:
            return None, None

        num_parent = [np.sum(y == c) for c in range(self.n_classes_)]
        best_gini = 1.0 - sum((n / m) ** 2 for n in num_parent)
        best_idx, best_thr = None, None

        for idx in range(n):
            thresholds, classes = zip(*sorted(zip(X[:, idx], y)))
            num_left = [0] * self.n_classes_
            num_right = num_parent.copy()
            for i in range(1, m):
                c = classes[i - 1]
                num_left[c] += 1
                num_right[c] -= 1
                gini_left = 1.0 - sum((num_left[x] / i) ** 2 for x in range(self.n_classes_))
                gini_right = 1.0 - sum((num_right[x] / (m - i)) ** 2 for x in range(self.n_classes_))
                gini = (i * gini_left + (m - i) * gini_right) / m
                if thresholds[i] == thresholds[i - 1]:
                    continue
                if gini < best_gini:
                    best_gini = gini
                    best_idx = idx
                    best_thr = (thresholds[i] + thresholds[i - 1]) / 2

        return best_idx, best_thr

    def _grow_tree(self, X, y, depth=0):
        num_samples_per_class = [np.sum(y == i) for i in range(self.n_classes_)]
        predicted_class = np.argmax(num_samples_per_class)
        node = Node(
            gini=self._gini(y),
            num_samples=len(y),
            num_samples_per_class=num_samples_per_class,
            predicted_class=predicted_class,
        )

        if depth < self.max_depth:
            idx, thr = self._best_split(X, y)
            if idx is not None:
                indices_left = X[:, idx] < thr
                X_left, y_left = X[indices_left], y[indices_left]
                X_right, y_right = X[~indices_left], y[~indices_left]
                node.feature_index = idx
                node.threshold = thr
                node.left = self._grow_tree(X_left, y_left, depth + 1)
                node.right = self._grow_tree(X_right, y_right, depth + 1)
        return node

    def _predict(self, inputs):
        node = self.tree_ #node=None
        while node.left:
            if (inputs[node.feature_index] < node.threshold):
                node = node.left
            else:
                node = node.right
        return node.predicted_class



class RandomForest:
    """
    A custom Random Forest Classifier.

    Parameters
    ----------
    n_trees : int, optional (default=100)
        The number of trees in the forest.
    max_depth : int, optional (default=10)
        The maximum depth of the tree.
    min_samples_leaf : int, optional (default=1)
        The minimum number of samples required to be at a leaf node.

    Attributes
    ----------
    trees : list
        The list of DecisionTreeClassifier instances forming the forest.

    Methods
    -------
    fit(X, y)
        Build a forest of trees from the training set (X, y).
    predict(X)
        Predict class for X by averaging predictions of all trees in the forest.
    """
    def __init__(self, n_trees=100, max_depth=10, min_samples_leaf=1):
        self.n_trees = n_trees
        self.max_depth = max_depth
        self.min_samples_leaf = min_samples_leaf
        self.trees = []

    def fit(self, X, y):
        # Create a bootstrap sample of the data.
        bootstraps_samples = []
        bootstraps_indices = []
        for _ in range(self.n_trees):
            bootstrap_indices = list(np.random.choice(range(len(X)), len(X), replace = True))
            #bootstrap_sample = X.sample(n=len(X), replace=True)
            bootstrap_sample = X.iloc[bootstrap_indices]
            bootstraps_samples.append(bootstrap_sample)
            bootstraps_indices.append(bootstrap_indices)

        # Train a decision tree on each bootstrap sample.
        for index, bootstrap_sample in enumerate(bootstraps_samples):
            tree = DecisionTreeClassifier(max_depth=self.max_depth, min_samples_leaf=self.min_samples_leaf)
            y_bootstrap = y.iloc[bootstraps_indices[index]]
            tree.fit(bootstrap_sample.values, y_bootstrap.values)
            self.trees.append(tree)

    def predict(self, X):
        # Make predictions for each data point using each decision tree in the forest.
        predictions = []
        for tree in self.trees:
            predictions.append(tree.predict(X))

        # Average the predictions of all the decision trees to get the final prediction.
        #final_predictions = np.mean(predictions, axis=0)
        final_predictions = stats.mode(predictions, axis=0)[0].flatten()
        return final_predictions

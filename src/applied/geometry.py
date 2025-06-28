from abc import ABC, abstractmethod
from typing import TypeVar, Generic


class Locatable (ABC):
    @abstractmethod
    def get_pos (self) -> 'Vec3D':
      pass


class Vec3D (Locatable):
    def __init__ (self, x, y, z):
        self.x = x
        self.y = y
        self.z = z
    
    def __mul__ (self, scalar):
        return Vec3D (self.x * scalar, self.y * scalar, self.z * scalar)

    def __rmul__ (self, scalar):
        return self.__mul__ (scalar)

    def __add__ (self, other):
        return Vec3D (self.x + other.x, self.y + other.y, self.z + other.z)

    def __sub__ (self, other):
        return Vec3D (self.x - other.x, self.y - other.y, self.z - other.z)

    def __getitem__(self, index):
        if not isinstance(index, int):
            raise TypeError(f"Index must be an integer, got {type(index)}")

        if index < 0 or index >= 3:
            raise IndexError(f"Index must be in range [0, 2], got {index}")

        return (self.x, self.y, self.z)[index]

    def dist_sq (self, other):
         return (self.x - other.x) ** 2 + (self.y - other.y) ** 2 + (self.z - other.z) ** 2

    def dist (self, other):
        return self.dist_sq (other) ** 0.5

    def get_pos (self) -> 'Vec3D':
        return self

    def to_tuple(self):
        return (self.x, self.y, self.z)

    @classmethod
    def from_tuple (cls, t):
        if len (t) != 3:
            raise ValueError (f"Tuple must have exactly 3 elements, got {len(t)}")
        return cls (t[0], t[1], t[2])


T = TypeVar ('T', bound = Locatable)
class KDTree (Generic[T]):
    class Node:
        def __init__ (self, items: list[T], axis = None, left = None, right = None):
            self.items = items
            self.axis = axis
            self.left = left
            self.right = right

    def __init__ (self, items: list[T], leaf_size = 5, max_depth = 10):
        self.items = items
        self.root = self.build_tree (items, 0, leaf_size, max_depth)
    
    # Build KDTree
    def build_tree (self, items, depth, leaf_size, max_depth):
        if (len (items) < leaf_size or depth > max_depth):
            return self.Node (items)
          
        axis = depth % 3
        items.sort (key = lambda item: item.get_pos ()[axis])
        median_val = items[len (items) // 2].get_pos ()[axis]

        # Split all items into children or cur
        left = []
        mid = []
        right = []

        for item in items:
            val = item.get_pos ()[axis]
            if val < median_val:
                left.append (item)
            elif val > median_val:
                right.append (item)
            else:
                mid.append (item)

        return self.Node (
            items = mid,
            axis = axis,
            left = self.build_tree (left, depth + 1, leaf_size, max_depth),
            right = self.build_tree (right, depth + 1, leaf_size, max_depth)
        )

    def search_radius (self, center, radius):
        found = []
        r_sq = radius ** 2

        def recurse (node):
            if node is None:
                return
            
            # If current split items are in, return
            for item in node.items:
                if (item.get_pos ().dist_sq (center) <= r_sq):
                    found.append (item)

            # Exit if leaf node
            if node.axis is None:
                return

            split_val = node.items[0].get_pos() [node.axis]
            
            if (center[node.axis] - radius < split_val):
                recurse (node.left)
            if (center[node.axis] + radius > split_val):
                recurse (node.right)
        
        recurse (self.root)
        return found
    
    def print_tree (self, node=None, depth=0):
        if node is None:
            node = self.root

        print(f"Node (depth = {depth}, size = {len (node.items)})")

        if node.left:
            self.print_tree (node.left, depth + 1)

        if node.right:
            self.print_tree (node.right, depth + 1)


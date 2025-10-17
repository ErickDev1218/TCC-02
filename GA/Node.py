class Node:

    def __init__(self, id, label = -1):
        self.value = id
        self.edges = []
        self.label = label 
        self.dominated = False

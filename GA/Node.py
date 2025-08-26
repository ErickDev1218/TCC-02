class Node:

    def __init__(self, id):
        self.value = id
        self.edges = []
        self.label = -1
        self.dominated = False

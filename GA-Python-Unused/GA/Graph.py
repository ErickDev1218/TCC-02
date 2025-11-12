from Node import Node

class Graph:

    def __init__(self, nodes=[], edges=[], labels = None):
        self.nodes = [Node(n) for n in nodes]
        self.edges = edges
        for edge in edges:
            node1 = next((node for node in self.nodes if node.value == edge[0]), None)
            node2 = next((node for node in self.nodes if node.value == edge[1]), None)
            if node1 and node2:
                node1.edges.append(edge)
                node2.edges.append(edge)
        if labels:
            for node in self.nodes:
                if node.value in labels:
                    node.label = labels[node.value]

    def print_graph(self):
        for node in self.nodes:
            print(f'Node {node.value} has edges: {node.edges}')

    def label_nodes(self, labels):
        for i,label in enumerate(labels):
            self.nodes[i].label = label
            self.nodes[i].dominated = True

        # print({node.value: node.label for node in self.nodes})        
        # for node in self.nodes:
        #     if node.label == -1:
        #         node.label = label
        #         label += 1
        #         self._label_connected_nodes(node, label)

    def _label_connected_nodes(self, node, label):
        for edge in node.edges:
            neighbor_value = edge[1] if edge[0] == node.value else edge[0]
            neighbor_node = next((n for n in self.nodes if n.value == neighbor_value), None)
            if neighbor_node and neighbor_node.label == -1:
                neighbor_node.label = label
                self._label_connected_nodes(neighbor_node, label)

    def get_labels(self):
        return {node.value: node.label for node in self.nodes}
    
    def get_dominated_status(self):
        return {node.value: node.dominated for node in self.nodes}
    
    def reset_labels_and_dominance(self):
        for node in self.nodes:
            node.label = -1
            node.dominated = False
    

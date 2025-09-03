from PRD import PRD
import random
class GA:

    def __init__(self, population_size, mutation_rate, crossover_rate, generations, graph):
        self.population_size = population_size
        self.mutation_rate = mutation_rate
        self.crossover_rate = crossover_rate
        self.generations = generations
        self.graph = graph
        self.prd = PRD(graph)

    def initialize_population(self):
        self.population = self.prd.randomized_initialization(self.population_size)
        self.population.append(self.prd.degree_based_initialization())

        print('Initial Population:', self.population)

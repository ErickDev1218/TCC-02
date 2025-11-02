from typing import List, Tuple
from PRD import PRD
import numpy as np

class GA:
    def __init__(self, population_size: int, mutation_rate: float, eletism_size: int, crossover_rate: float, generations: int, graph, config: List[str]):
        self.population_size = population_size
        self.mutation_rate = mutation_rate
        self.eletism_size = eletism_size
        self.crossover_rate = crossover_rate
        self.generations = generations
        self.graph = graph
        self.config = config
        self.population = None
        self.prd = PRD(graph)
        self.current_fitness = np.full(population_size + 1, -1)
        self.initialize_population()
       

    def calculate_fitness(self, individual: np.ndarray) -> float:
        return sum(individual) + (0 if self.prd.check_prd_v2(individual) else len(individual) * 5)  # Penalidade para soluções inválidas

    def initialize_population(self):
        self.population = self.prd.randomized_initialization(self.population_size)
        self.population = np.vstack([self.population, self.prd.degree_based_initialization()])
        valid_mask = self.prd.check_prd_v3(self.population)  # hipotético: retorna array booleano
        penalty = np.where(valid_mask, 0, self.population.shape[1] * 5)
        self.current_fitness = np.sum(self.population, axis=1) + penalty
        

    def __randomized_selection(self):
        # Implement selection logic using NumPy
        print('population \n', self.population)
        n = self.population.shape[0]
        pairs = []
        for _ in range(n // 2):
            # Seleciona o primeiro indivíduo do par
            idxs1 = np.random.choice(n, 3, replace=False)
            winner1_idx = idxs1[np.argmin(self.current_fitness[idxs1])]
            
            # Seleciona o segundo indivíduo do par, garantindo que seja diferente do primeiro
            while True:
                idxs2 = np.random.choice(n, 3, replace=False)
                winner2_idx = idxs2[np.argmin(self.current_fitness[idxs2])]
                if winner2_idx != winner1_idx:
                    print('index', winner1_idx, winner2_idx)
                    break
            print('selected pair \n',self.population[winner1_idx], self.population[winner2_idx])
            pairs.append((self.population[winner1_idx], self.population[winner2_idx]))
        return np.array(pairs)

    def __one_point_crossover(self, pair : Tuple[np.ndarray, np.ndarray]) -> Tuple[np.ndarray, np.ndarray]:
        if np.random.rand() < self.crossover_rate:
            parent1, parent2 = pair
            point = np.random.randint(1, len(parent1) - 1)
            print('crossover point:', point)
            child1 = np.concatenate((parent1[:point], parent2[point:]))
            child2 = np.concatenate((parent2[:point], parent1[point:]))
            return (child1, child2)
        return pair

    def mutate(self, individual: np.ndarray) -> np.ndarray:
        for i in range(len(individual)):
            if np.random.rand() < self.mutation_rate:
                individual[i] = 1 - individual[i]  # Flip the bit
        return individual

    def ga_flow(self):
        # print('check', self.prd.check_prd_v3(self.population))
        # valid_mask = self.prd.check_prd_v3(self.population)  # hipotético: retorna array booleano
        # penalty = np.where(valid_mask, 0, self.population.shape[1] * 5)
        # self.current_fitness = np.sum(self.population, axis=1) + penalty
        # self.current_fitness = [self.calculate_fitness(ind) for ind in self.population]
        # self.current_fitness = np.sum(self.population, axis=1)
        # self.current_fitness = np.array([
        #     np.sum(ind) + (0 if self.prd.check_prd_v2(ind) else len(ind) * 5)
        #     for ind in self.population
        # ])
        # print('population', self.population)
        # print('actual fitness', self.current_fitness)
        # for generation in range(self.generations):
        #     # Selection
        #     selected = self.selection()
        #     # Crossover and Mutation
        #     new_population = []
        #     for i in range(0, len(selected), 2):
        #         parent1 = selected[i]
        #         parent2 = selected[i + 1] if i + 1 < len(selected) else selected[0]
        #         child1, child2 = self.crossover(parent1, parent2)
        #         new_population.append(self.mutate(child1))
        #         new_population.append(self.mutate(child2))
        print(self.generations)
        for _ in range(self.generations):
            selectted_pairs = self.__randomized_selection()
            print('selected',selectted_pairs)
            new_population = []
            for pair in selectted_pairs:
                child1, child2 = self.__one_point_crossover((pair[0], pair[1]))
                print('pairs:',pair[0], pair[1])
                # print('parent', pair)
                print('chields \n', child1, child2)
                # new_population.append(self.mutate(child1))
                # new_population.append(self.mutate(child2))
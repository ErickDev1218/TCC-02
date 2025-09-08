from PRD import PRD
from Solution import Solution
import random

class GA:

    def __init__(self, population_size, mutation_rate, eletism_size, crossover_rate, generations, graph, config):
        self.population_size = population_size
        self.mutation_rate = mutation_rate
        self.eletism_size = eletism_size
        self.crossover_rate = crossover_rate
        self.generations = generations
        self.graph = graph
        self.prd = PRD(graph)
        self.solutions = []

        self.selection_func, self.crossover_func, self.mutate_func, self.eletism_func = self.config_ga(config)

    def config_ga(self, config):

        #### SELECTION ####
        if config[0] == "tournament":
            selection = self.__tournament_selection
        elif config[0] == "roullette":
            selection = self.__roullette_wheel_selection
        
        #### CROSSOVER ####
        if config[1] == "one_point":
            crossover = self.__one_point_crossover
        
        #### MUTATE ####
        if config[2] == "random":
            mutate = self.__random_mutate
        elif config[2] == "shift":
            mutate = self.__shift_mutate

        #### ELETISM ####
        if config[3] == "eletism":
            eletism = self.__default_eletism
        elif config[3] == "partition":
            eletism = self.__partition_based_eletism
            
        return selection, crossover, mutate, eletism


    def ga_flow(self):
        for i in range(self.generations):
            orderned_solutions = sorted(self.solutions, key=lambda s: s.fitness, reverse=True)
            self.selection = self.selection_func(orderned_solutions)
            self.childrens = []
            for pair in self.selection:
                child_one, child_two  = self.crossover_func(pair)
                self.childrens.append(child_one)
                self.childrens.append(child_two)
            for i in range(len(self.childrens)):
                self.childrens[i] = self.mutate_func(self.childrens[i])

            next_generation = self.eletism_func(self.solutions, self.childrens)
            for ind in next_generation:
                ind.show_solution()

            self.solutions = next_generation
        
        print("Final Solutions:")
        for sol in self.solutions:
            sol.show_solution()
    
    def initialize_population(self):
        self.population = self.prd.randomized_initialization(self.population_size)
        self.population.append(self.prd.degree_based_initialization())
        for ind in self.population:
            isValid = self.prd.check_prd(ind)
            self.solutions.append(Solution(ind, isValid))


    ################ SELECTION ################

    def __tournament_selection(self, solutions, tournament_size=3):
        pairs = []
        for _ in range(len(solutions) // 2):
            # Seleciona o primeiro indivíduo do par
            tournament1 = random.sample(solutions, tournament_size)
            winner1 = min(tournament1, key=lambda s: s.fitness)

            # Seleciona o segundo indivíduo do par, garantindo que seja diferente do primeiro
            while True:
                tournament2 = random.sample(solutions, tournament_size)
                winner2 = min(tournament2, key=lambda s: s.fitness)
                if winner2 != winner1:
                    break

            pairs.append((winner1, winner2))
        return pairs
    
    def __roullette_wheel_selection(self, solutions):
        pairs = []
        probabilities = [1 / s.fitness for s in solutions]
        total_prob = sum(probabilities)
        probabilities = [p / total_prob for p in probabilities]

        for _ in range(len(solutions) // 2):
            parent1 = random.choices(solutions, weights=probabilities, k=1)[0]
            parent2 = parent1
            while parent2 == parent1:
                parent2 = random.choices(solutions, weights=probabilities, k=1)[0]
            pairs.append((parent1, parent2))
        return pairs
    

    ################ CROSSOVER ################

    def __one_point_crossover(self,pair):
        if random.random() < self.crossover_rate:
            point = random.randint(1, len(pair[0].solution) - 1)
            child1_solution = pair[0].solution[:point] + pair[1].solution[point:]
            child2_solution = pair[1].solution[:point] + pair[0].solution[point:]

            child1_isValid = self.prd.check_prd(child1_solution)
            child2_isValid = self.prd.check_prd(child2_solution)

            return Solution(child1_solution, child1_isValid), Solution(child2_solution, child2_isValid)
        else:
            return pair[0], pair[1]
        
    ################ MUTATION ################

    def __random_mutate(self, solution):
        if random.random() < self.mutation_rate:
            mutated_solution = solution.solution[:]
            for i in range(len(mutated_solution)):
                mutated_solution[i] = random.choice([0, 1, 2])
            isValid = self.prd.check_prd(mutated_solution)
            return Solution(mutated_solution, isValid)
        else:
            return solution
    
    def __shift_mutate(self, solution):
        if random.random() < self.mutation_rate:
            mutated_solution = solution.solution[:]
            if len(mutated_solution) > 1:
                i, j = random.sample(range(len(mutated_solution)), 2)
                # Faz o swap
                mutated_solution[i], mutated_solution[j] = mutated_solution[j], mutated_solution[i]
            isValid = self.prd.check_prd(mutated_solution)
            return Solution(mutated_solution, isValid)
        else:
            return solution

    
    ################ ELETISM ################

    def __default_eletism(self, current_population, new_population):
        # Ordena as populações pela aptidão
        current_sorted = sorted(current_population, key=lambda s: s.fitness, reverse=True)
        new_sorted = sorted(new_population, key=lambda s: s.fitness, reverse=True)

        # Mantém os melhores indivíduos da população atual
        next_generation = current_sorted[:self.eletism_size]

        # Preenche o restante da nova população com os melhores da nova geração
        next_generation += new_sorted[:len(current_population) - self.eletism_size]

        return next_generation
    
    def __partition_based_eletism(self, current_population, new_population):
        combined = current_population + new_population
        combined_sorted = sorted(combined, key=lambda s: s.fitness)
        n = len(current_population)
        quarter = n // 4
        # Seleciona o top 1/4 e os próximos 3/4 melhores
        next_generation = combined_sorted[:quarter] + combined_sorted[quarter:quarter + (n - quarter)]
        return next_generation
def vectorized_selection(population, fitness_scores, num_parents):
    parents_indices = np.argsort(fitness_scores)[-num_parents:]
    return population[parents_indices]

def crossover(parent1, parent2):
    crossover_point = np.random.randint(1, len(parent1) - 1)
    child1 = np.concatenate((parent1[:crossover_point], parent2[crossover_point:]))
    child2 = np.concatenate((parent2[:crossover_point], parent1[crossover_point:]))
    return child1, child2

def mutate(individual, mutation_rate):
    mutation_mask = np.random.rand(len(individual)) < mutation_rate
    individual[mutation_mask] = np.random.randint(0, 2, size=np.sum(mutation_mask))
    return individual

def initialize_population(population_size, num_genes):
    return np.random.randint(0, 2, size=(population_size, num_genes))
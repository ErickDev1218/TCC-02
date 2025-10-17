
class Solution:

    def __init__(self, solution, isValid):
        self.solution = solution
        self.isValid = isValid
        self.fitness = self._calculate_fitness()

    def _calculate_fitness(self):
        return sum(self.solution) + (0 if self.isValid else len(self.solution) * 5)  # Penalidade para soluções inválidas
    
    def show_solution(self):
        print(f'Solution: {self.solution}, Fitness: {self.fitness}, Valid: {self.isValid}')
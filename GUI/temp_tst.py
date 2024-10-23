import numpy as np
import matplotlib.pyplot as plt

# Parameters
dt = 0.01  # time step
num_steps = 500  # number of steps in the simulation
num_particles = 10  # number of particles
radius = 0.05  # radius of particles
gravity = np.array([0, -9.81])  # gravitational acceleration

# Initialize particle properties
positions = np.random.rand(num_particles, 2)  # random positions
velocities = np.zeros((num_particles, 2))  # initial velocities
masses = np.ones(num_particles)  # all particles have the same mass

# Function to detect collisions and update velocities
def handle_collisions(positions, velocities):
    for i in range(num_particles):
        for j in range(i + 1, num_particles):
            dist = np.linalg.norm(positions[i] - positions[j])
            if dist < 2 * radius:
                # Simple elastic collision
                normal = (positions[i] - positions[j]) / dist
                relative_velocity = velocities[i] - velocities[j]
                impulse = 2 * np.dot(relative_velocity, normal) / (masses[i] + masses[j])
                velocities[i] -= impulse * normal / masses[i]
                velocities[j] += impulse * normal / masses[j]
    return velocities

# Simulation loop
for step in range(num_steps):
    # Update velocities
    velocities += gravity * dt

    # Update positions
    positions += velocities * dt

    # Handle collisions
    velocities = handle_collisions(positions, velocities)

    # Boundary conditions (particles bounce off the walls)
    for i in range(num_particles):
        if positions[i, 0] < radius or positions[i, 0] > 1 - radius:
            velocities[i, 0] *= -1
        if positions[i, 1] < radius or positions[i, 1] > 1 - radius:
            velocities[i, 1] *= -1

    # Plot particles
    plt.clf()
    plt.xlim(0, 1)
    plt.ylim(0, 1)
    plt.gca().set_aspect('equal', adjustable='box')
    plt.scatter(positions[:, 0], positions[:, 1], s=(radius * 1000) ** 2)
    plt.pause(0.01)

plt.show()

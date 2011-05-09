#ifndef WAVECONSTANTS_H
#define WAVECONSTANTS_H

// The speed of each wave particle, in world coordinates per second
#define WAVE_SPEED 10

// The minimum amplitude of each particle, in world coordinates (distance from projection grid plane).
// When a particle's amplitude is less than WAVE_MIN_AMPLITUDE, it dies
#define WAVE_MIN_AMPLITUDE .000625

// The maximum amplitude any particle may have. For rendering purposes.
#define WAVE_MAX_AMPLITUDE 5

// The rate at which the amplitude of a particle decreases, in world coordinates (distance
// from the projection grid plane) per second
#define WAVE_AMPLITUDE_FALLOFF 0.f

// When multiplied by the radius of a wave particle, yields the maximum distance (in world coordinates)
// between neighboring wave particles (i.e. before subidivision occurs)
#define WAVE_SUBDIVISION_COEFFICIENT .5

// The maximum number of wave particles to pre-allocate for the entire simulation
#define WAVE_PARTICLE_COUNT 100000

// Heightmap width in world space (centered at the origin)
#define WAVE_HEIGHTMAP_WIDTH  512

// Heightmap height in world space (centered at the origin)
#define WAVE_HEIGHTMAP_HEIGHT 512

// The resolution of the heightmap, in pixels
#define WAVE_HEIGHTMAP_RESOLUTION 1024

// The 'area of effect' of a single wave particle
#define WAVE_PARTICLE_RADIUS 1.f

// The radius of the convolution kernel (in heightmap pixels) during heightmap rendering
// ceil(worldspace_to_heightmapspace(particle `radius)) =
// ceil(WAVE_HEIGHTMAP_RESOLUTION / WAVE_HEIGHTMAP_WIDTH * WAVE_PARTICLE_RADIUS) =
// ceil(1024 / 512 * 1) =
// 2
#define WAVE_CONVOLUTION_KERNEL_RADIUS 2

// The number of heightmap pixels over which the particles are convolved during heightmap rendering
#define WAVE_CONVOLUTION_KERNEL_WIDTH (WAVE_CONVOLUTION_KERNEL_RADIUS + 1)

#endif // WAVECONSTANTS_H

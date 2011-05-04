#ifndef WAVECONSTANTS_H
#define WAVECONSTANTS_H

// The speed of each wave particle, in world coordinates per second
#define WAVE_SPEED 10

// The minimum amplitude of each particle, in world coordinates (distance from projection grid plane).
// When a particle's amplitude is less than WAVE_MIN_AMPLITUDE, it dies
#define WAVE_MIN_AMPLITUDE .1

// The maximum amplitude any particle may have. For rendering purposes.
#define WAVE_MAX_AMPLITUDE 50

// The rate at which the amplitude of a particle decreases, in world coordinates (distance
// from the projection grid plane) per second
#define WAVE_AMPLITUDE_FALLOFF .25f

// When multiplied by the radius of a wave particle, yields the maximum distance (in world coordinates)
// between neighboring wave particles (i.e. before subidivision occurs)
// TODO: read the paper for the suggested value
#define WAVE_SUBDIVISION_COEFFICIENT 1

// The maximum number of wave particles to pre-allocate for the entire simulation
#define WAVE_PARTICLE_COUNT 100000

// Heightmap width in world space (centered at the origin)
#define WAVE_HEIGHTMAP_WIDTH  100

// Heightmap height in world space (centered at the origin)
#define WAVE_HEIGHTMAP_HEIGHT 100

#define WAVE_HEIGHTMAP_RESOLUTION 256

#endif // WAVECONSTANTS_H
